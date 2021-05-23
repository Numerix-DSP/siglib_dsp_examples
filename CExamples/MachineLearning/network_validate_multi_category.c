// SigLib Neural Network Verification Program
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <siglib_host_utils.h>                              // Optionally includes conio.h and time.h subset functions
#include <siglib.h>                                         // SigLib DSP library
#include <gnuplot_c.h>                                      // Gnuplot/C


// Define constants
#ifndef NETWORK_INPUT_SAMPLE_LENGTH
#define NETWORK_INPUT_SAMPLE_LENGTH 128
#endif
#ifndef NETWORK_HIDDEN_LAYER_NODES
#define NETWORK_HIDDEN_LAYER_NODES  25
#endif

#define NUM_CATEGORIES              4                       // Number of output categories


// Declare global variables and arrays
static SLData_t         *pLayer1PostActivation, *pLayer2PostActivation;
static SLArrayIndex_t   *pCategoricalValue;

static char             filename0[512];
static char             filename1[512];
static char             filename2[512];
static char             filename3[512];
static char             filenameWeights[2400];

static SLData_t         confusionMatrix[NUM_CATEGORIES][NUM_CATEGORIES];    //            Predicted Categories
                                                                            // Actual         00, 01 ...
                                                                            // Categories     10, 11 ...
                                                                            //                  ...

static SLData_t layer1Weights[NETWORK_HIDDEN_LAYER_NODES][NETWORK_INPUT_SAMPLE_LENGTH]; // Declare weights for two transition paths
static SLData_t layer2Weights[NUM_CATEGORIES][NETWORK_HIDDEN_LAYER_NODES];

#ifndef ACTIVATION_TYPE
    #define ACTIVATION_TYPE                 SIGLIB_ACTIVATION_TYPE_LOGISTIC // Activation type
    #define ACTIVATION_ALPHA                SIGLIB_ZERO                     // Alpha value not required in this mode
#else
    #if (ACTIVATION_TYPE == 2)
        #define ACTIVATION_ALPHA            0.01            // Leaky ReLU alpha value
    #else
        #define ACTIVATION_ALPHA            SIGLIB_ZERO     // Alpha value not required in this mode
    #endif
#endif

#define MAGNITUDE_SCALING_FACTOR            (320.)          // Scaling factor used to avoid numerical overflow in the exponent function

SLArrayIndex_t  confusionMatrixFlag = 0;
SLArrayIndex_t  epochCount = 1;
SLArrayIndex_t  debugFlag = 0;

void show_help (void);
void parse_command_line (int argc, char *argv[]);

int main (int argc, char *argv[])
{
    pLayer1PostActivation = SUF_VectorArrayAllocate (NETWORK_HIDDEN_LAYER_NODES);   // Allocate arrays
    pLayer2PostActivation = SUF_VectorArrayAllocate (NUM_CATEGORIES);

    if ((NULL == pLayer1PostActivation) || (NULL == pLayer2PostActivation)) {
        printf ("\n\nMemory allocation failed\n\n");
        exit (-1);
    }

    filename0[0] = 0;
    filename1[0] = 0;
    filename2[0] = 0;
    filename3[0] = 0;

    parse_command_line (argc, argv);                        //  Parse command line options

    if (0 == (filename0[0]) || 0 == (filename1[0]) || 0 == (filename2[0]) || 0 == (filename3[0])) {
        printf("Usage error:\n");
        show_help ();
        exit (0);
    }


    printf("\nValidating The Network ...\n");
    printf("    Filename #0    : %s\n", filename0);
    printf("    Filename #1    : %s\n", filename1);
    printf("    Filename #2    : %s\n", filename2);
    printf("    Filename #3    : %s\n", filename3);
    printf("    Activation Type: %d\n\n", (int)ACTIVATION_TYPE);


    // Load validation data
    SLArrayIndex_t nRows, nCols;
    SLData_t *pValidationData = NULL;                       // Pointer only - memory will be allocated in SUF_CsvReadMatrix()
    SUF_CsvReadMatrix (&pValidationData, "Vdata.csv", SIGLIB_FIRST_ROW_KEEP, &nRows, &nCols);

    if ((nRows == 0) || (NULL == pValidationData)) {
        printf ("\n\nSUF_CsvReadMatrix (Vdata.csv) failure!.\n\n");
        exit (-1);
    }

    if ((nCols-1) != NETWORK_INPUT_SAMPLE_LENGTH) {
        printf ("Error - Validation sequences must be %d samples long\n", NETWORK_INPUT_SAMPLE_LENGTH);
        printf ("        Provided sequence was %d samples long\n", nCols);
        exit(-1);
    }

    if (debugFlag == 1) {
        printf("nCols = %d, NETWORK_INPUT_SAMPLE_LENGTH = %d\n", nCols-1, NETWORK_INPUT_SAMPLE_LENGTH);

        printf("Number of validation sequences:   %d\n", nRows);
        printf ("Validation Data\n");
        SUF_PrintMatrix (pValidationData, nRows, NETWORK_INPUT_SAMPLE_LENGTH);
    }

    pCategoricalValue = SUF_IndexArrayAllocate(nRows);       // Allocate the categorical value array
    if (NULL == pCategoricalValue) {
        printf ("\n\nMemory allocation failed (pCategoricalValue)\n\n");
        exit (-1);
    }

    SMX_ExtractCategoricalColumn (pValidationData,          // Pointer to source matrix
                                  pCategoricalValue,        // Pointer to destination array
                                  nRows,                    // Number of rows in matrix
                                  nCols);                   // Number of columns in matrix
    SMX_DeleteOldColumn (pValidationData,                   // Pointer to source matrix
                         pValidationData,                   // Pointer to destination matrix
                         nCols-1,                           // Column number to delete
                         nRows,                             // Number of rows in matrix
                         nCols);                            // Number of columns in matrix

    nCols--;                                                // We have removed the categorical value column

                                                            // Scale all samples to avoid overflow in exponent function
    SDA_Multiply (pValidationData,                          // Pointer to source array
                  SIGLIB_ONE/MAGNITUDE_SCALING_FACTOR,      // Scalar value
                  pValidationData,                          // Pointer to destination array
                  nRows*nCols);                             // Array length


#if defined (_MSC_VER)                                      // Defined by Microsoft compilers
    sprintf (filenameWeights, "weightCoefficientsFiles\\weights%s_%s_%s_%s.dat", filename0, filename1, filename2, filename3);
#else
    sprintf (filenameWeights, "weightCoefficientsFiles/weights%s_%s_%s_%s.dat", filename0, filename1, filename2, filename3);
#endif

    int errorCode =
        SUF_ReadWeightsBinaryFile (filenameWeights,             // Filename
                                   (SLData_t *)layer1Weights,   // layer1Weights
                                   (SLData_t *)layer2Weights);  // layer2Weights

    if (errorCode <= 0) {
        printf ("Error reading weights file !! : %s\n", filenameWeights);
        printf ("Error Code: %d\n", errorCode);
        exit (errorCode);
    }

    if (debugFlag == 1) {
        printf ("\nweightCoefficientsFiles/weights%s_%s_%s_%s.dat\n", filename0, filename1, filename2, filename3);
        printf("layer1Weights\n"); SUF_PrintMatrix((SLData_t *)layer1Weights, NETWORK_HIDDEN_LAYER_NODES, NETWORK_INPUT_SAMPLE_LENGTH);
        printf("layer2Weights\n"); SUF_PrintMatrix((SLData_t *)layer2Weights, NUM_CATEGORIES, NETWORK_HIDDEN_LAYER_NODES);
        printf("\n");
        printf ("Absolute Max layer1Weights: %lf\n", SDA_AbsMax((SLData_t *)layer1Weights, NETWORK_HIDDEN_LAYER_NODES*NETWORK_INPUT_SAMPLE_LENGTH));  // Abs max value
        printf ("Absolute Max layer2Weights: %lf\n", SDA_AbsMax((SLData_t *)layer2Weights, NUM_CATEGORIES*NETWORK_HIDDEN_LAYER_NODES));
        printf("\n\n");
    }

    SLArrayIndex_t totalClassificationCount = SIGLIB_AI_ZERO;
    SLArrayIndex_t correctClassificationCount = SIGLIB_AI_ZERO;
    SLArrayIndex_t incorrectClassificationCount = SIGLIB_AI_ZERO;

    SDA_Clear ((SLData_t *) confusionMatrix,                // Array pointer
               NUM_CATEGORIES*NUM_CATEGORIES);              // Array length

    for (SLArrayIndex_t validationSequenceNumber = 0; validationSequenceNumber < nRows; validationSequenceNumber++) {
        SLArrayIndex_t predictedCategory =
            SDA_TwoLayerNCategoryNetworkPredict (pValidationData+(validationSequenceNumber*NETWORK_INPUT_SAMPLE_LENGTH),    // Pointer to data to validate
                                                 (SLData_t *)layer1Weights,                                                 // Pointer to layer #1 weights
                                                 (SLData_t *)layer2Weights,                                                 // Pointer to layer #2 weights
                                                 pLayer1PostActivation,                                                     // Pointer to post activation for hidden layer
                                                 pLayer2PostActivation,                                                     // Pointer to post activation for output layer
                                                 ACTIVATION_TYPE,                                                           // Hidden layer activation type
                                                 ACTIVATION_ALPHA,                                                          // Hidden layer alpha value
                                                 ACTIVATION_TYPE,                                                           // Output layer activation type
                                                 ACTIVATION_ALPHA,                                                          // Output layer alpha value
                                                 NETWORK_INPUT_SAMPLE_LENGTH,                                               // Input sample length
                                                 NETWORK_HIDDEN_LAYER_NODES,                                                // Hidden layer length
                                                 NUM_CATEGORIES);                                                           // Number of output categories

        totalClassificationCount++;
        if (predictedCategory == pCategoricalValue[validationSequenceNumber]) {   // If correct detection then increment counter
            correctClassificationCount++;
        }
        else {                                                  // If result incorrect
            incorrectClassificationCount++;
        }

        confusionMatrix[pCategoricalValue[validationSequenceNumber]][predictedCategory] = confusionMatrix[(SLArrayIndex_t)pCategoricalValue[validationSequenceNumber]][predictedCategory] + SIGLIB_ONE;
    }

    printf("\n%s, %s, %s, %s\n\n", filename0, filename1, filename2, filename3);

    printf("Confusion Matrix For: %s, %s, %s, %s\n", filename0, filename1, filename2, filename3);
    printf("           Predicted Category\n");
    printf("                0    1    2    3\n");
    printf("           0   %d    %d    %d    %d\n", (int)confusionMatrix[0][0], (int)confusionMatrix[0][1], (int)confusionMatrix[0][2], (int)confusionMatrix[0][3]);
    printf("Actual     1   %d    %d    %d    %d\n", (int)confusionMatrix[1][0], (int)confusionMatrix[1][1], (int)confusionMatrix[1][2], (int)confusionMatrix[1][3]);
    printf("Category   2   %d    %d    %d    %d\n", (int)confusionMatrix[2][0], (int)confusionMatrix[2][1], (int)confusionMatrix[2][2], (int)confusionMatrix[2][3]);
    printf("           3   %d    %d    %d    %d\n", (int)confusionMatrix[3][0], (int)confusionMatrix[3][1], (int)confusionMatrix[3][2], (int)confusionMatrix[3][3]);
    printf("\n");

    printf("Confusion Matrix (Percentage) For: %s, %s, %s, %s\n", filename0, filename1, filename2, filename3);
    printf("           Predicted Category\n");
    printf("                0    1    2    3\n");
    printf("           0   %d%%   %d%%   %d%%   %d%%\n", (int)(confusionMatrix[0][0]*100./totalClassificationCount), (int)(confusionMatrix[0][1]*100./totalClassificationCount), (int)(confusionMatrix[0][2]*100./totalClassificationCount), (int)(confusionMatrix[0][3]*100./totalClassificationCount));
    printf("Actual     1   %d%%   %d%%   %d%%   %d%%\n", (int)(confusionMatrix[1][0]*100./totalClassificationCount), (int)(confusionMatrix[1][1]*100./totalClassificationCount), (int)(confusionMatrix[1][2]*100./totalClassificationCount), (int)(confusionMatrix[1][3]*100./totalClassificationCount));
    printf("Category   2   %d%%   %d%%   %d%%   %d%%\n", (int)(confusionMatrix[2][0]*100./totalClassificationCount), (int)(confusionMatrix[2][1]*100./totalClassificationCount), (int)(confusionMatrix[2][2]*100./totalClassificationCount), (int)(confusionMatrix[2][3]*100./totalClassificationCount));
    printf("           3   %d%%   %d%%   %d%%   %d%%\n", (int)(confusionMatrix[3][0]*100./totalClassificationCount), (int)(confusionMatrix[3][1]*100./totalClassificationCount), (int)(confusionMatrix[3][2]*100./totalClassificationCount), (int)(confusionMatrix[3][3]*100./totalClassificationCount));
    printf("\n");

    SLData_t predictionAccuracy  = (SLData_t)correctClassificationCount / (SLData_t)nRows;
    SLData_t predictionErrorRate = (SLData_t)incorrectClassificationCount / (SLData_t)nRows;

    printf ("Statistics\n");
    printf ("Prediction Accuracy                              : %lf\n", predictionAccuracy);
    printf ("Prediction Error Rate (Misclassification Rate)   : %lf\n", predictionErrorRate);
    printf ("Prediction Accuracy %%                            : %lf\n", predictionAccuracy*SIGLIB_ONE_HUNDRED);
    printf ("Prediction Error Rate (Misclassification Rate) %% : %lf\n", predictionErrorRate*SIGLIB_ONE_HUNDRED);

    if (confusionMatrixFlag) {
        // printf ("totalActualClassZeroCount = %d\n", totalActualClassZeroCount);
        // printf ("totalActualClassOneCount  = %d\n", totalActualClassOneCount);

        printf("Plotting Confusion Matrix\n");
        h_GPC_Plot      *hConfusionMatrix;                  // Plot object
        h_GPC_Plot      *hConfusionMatrixPercentage;        // Plot object
        hConfusionMatrix =
            gpc_plot_confusion_matrix ((SLData_t *)confusionMatrix, // Confusion matrix
                                       totalClassificationCount,    // Maximum value
                                       NUM_CATEGORIES);             // Number of categories

                                                            // Convert to percentage
        SDA_Multiply ((SLData_t *)confusionMatrix,                      // Input array
                      SIGLIB_ONE_HUNDRED / totalClassificationCount,    // Scalar
                      (SLData_t *)confusionMatrix,                      // Output array
                      NUM_CATEGORIES*NUM_CATEGORIES);                   // Array length

        hConfusionMatrixPercentage =
            gpc_plot_confusion_matrix_percentage ((SLData_t *)confusionMatrix,  // Confusion matrix
                                                  NUM_CATEGORIES);              // Number of categories

        printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
        gpc_close(hConfusionMatrix);
        gpc_close(hConfusionMatrixPercentage);
    }

    SUF_MemoryFree (pLayer1PostActivation);                 // Free memory
    SUF_MemoryFree (pLayer2PostActivation);
    SUF_MemoryFree (pValidationData);
    SUF_MemoryFree (pCategoricalValue);

    exit (0);
}


void parse_command_line (int argc, char *argv[])

{
    for (int argNum = 1; argNum < argc; argNum++) {
        if (*(argv[argNum]) == '-') {
            switch (*(argv[argNum]+1)) {                    // Get command letter
                case 'e':
                    epochCount = atoi(argv[argNum+1]);
                    argNum++;
                    break;
                case 'd':
                    debugFlag = 1;
                    printf("Debug information = True\n");
                    break;
                case '0':
                    strcpy (filename0, argv[argNum+1]);
                    argNum++;
                    break;
                case '1':
                    strcpy (filename1, argv[argNum+1]);
                    argNum++;
                    break;
                case '2':
                    strcpy (filename2, argv[argNum+1]);
                    argNum++;
                    break;
                case '3':
                    strcpy (filename3, argv[argNum+1]);
                    argNum++;
                    break;
                case 'm':
                    confusionMatrixFlag = 1;
                    printf("Displaying confusion matrix = True\n");
                    break;
                case 'h':
                    show_help ();
                    exit (0);
                    break;
                default:
                    printf ("Invalid parameter combination\n");
                    show_help ();
                    exit (0);
                    break;
            }
        }
        else {
            printf ("Invalid parameter combination\n");
            show_help ();
            exit (0);
        }
    }
}


void show_help (void)

{
    printf ("Usage:\n");
    printf ("network_validate_multi_category [params]\n");
    printf ("\t-0 filename   Filename #0 (Required)\n");
    printf ("\t-1 filename   Filename #1 (Required)\n");
    printf ("\t-2 filename   Filename #2 (Required)\n");
    printf ("\t-3 filename   Filename #3 (Required)\n");
    printf ("\t-e Number     Epoch Count\n");
    printf ("\t-m            Display confustion matrix\n");
    printf ("\t-d            Display debug information\n");
    printf ("\t-h            Help\n");
}

