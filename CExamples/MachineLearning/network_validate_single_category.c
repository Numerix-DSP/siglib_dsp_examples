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

#define CLASSIFICATION_THRESHOLD    (0.5)

#define NUM_CATEGORIES              2                       // Number of output categories


// Declare global variables and arrays
static SLArrayIndex_t   *pCategoricalValue;
static SLData_t         *pLayer1PostActivation;

static char             filename0[512];
static char             filename1[512];
static char             filenameWeights[1200];

static SLData_t         confusionMatrix[NUM_CATEGORIES][NUM_CATEGORIES];    //            Predicted Categories
                                                                            // Actual         00, 01 ...
                                                                            // Categories     10, 11 ...
                                                                            //                  ...

static SLData_t layer1Weights[NETWORK_HIDDEN_LAYER_NODES][NETWORK_INPUT_SAMPLE_LENGTH]; // Declare weights for two transition paths
static SLData_t layer2Weights[NETWORK_HIDDEN_LAYER_NODES];

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


SLArrayIndex_t  scatterDiagramFlag = 0;
SLArrayIndex_t  confusionMatrixFlag = 0;
SLArrayIndex_t  epochCount = 1;
SLArrayIndex_t  debugFlag = 0;

void show_help (void);
void parse_command_line (int argc, char *argv[]);

int main (int argc, char *argv[])

{
    pLayer1PostActivation = SUF_VectorArrayAllocate (NETWORK_HIDDEN_LAYER_NODES);   // Allocate arrays
    if (NULL == pLayer1PostActivation) {
        printf ("\n\nMemory allocation failed\n\n");
        exit (-1);
    }

    filename0[0] = 0;
    filename1[0] = 0;

    parse_command_line (argc, argv);                        //  Parse command line options

    if (0 == (filename0[0]) || 0 == (filename1[0])) {
        printf("Usage error:\n");
        show_help ();
        exit (0);
    }


    printf("\nValidating The Network ...\n");
    printf("    Filename #0    : %s\n", filename0);
    printf("    Filename #1    : %s\n", filename1);
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
    sprintf (filenameWeights, "weightCoefficientsFiles\\weights%s_%s.dat", filename0, filename1);
#else
    sprintf (filenameWeights, "weightCoefficientsFiles/weights%s_%s.dat", filename0, filename1);
#endif

    int errorCode =
        SUF_ReadWeightsBinaryFile (filenameWeights,             // Filename
                                   (SLData_t *)layer1Weights,   // layer1Weights array
                                   layer2Weights);              // layer2Weights array

    if (errorCode <= 0) {
        printf ("Error reading weights file !! : %s\n", filenameWeights);
        printf ("Error Code: %d\n", errorCode);
        exit (errorCode);
    }

    if (debugFlag == 1) {
        printf ("\nweightCoefficientsFiles/weights%s_%s.dat\n", filename0, filename1);
        printf("layer1Weights\n"); SUF_PrintMatrix((SLData_t *)layer1Weights, NETWORK_HIDDEN_LAYER_NODES, NETWORK_INPUT_SAMPLE_LENGTH);
        printf("layer2Weights\n"); SUF_PrintMatrix((SLData_t *)layer2Weights, 1, NETWORK_HIDDEN_LAYER_NODES);
        printf("\n\n");
    }

    SLArrayIndex_t totalClassificationCount = SIGLIB_AI_ZERO;
    SLArrayIndex_t correctClassificationCount = SIGLIB_AI_ZERO;
    SLArrayIndex_t totalActualClassZeroCount = SIGLIB_AI_ZERO;
    SLArrayIndex_t totalActualClassOneCount = SIGLIB_AI_ZERO;
    ComplexRect_s * xyData = (ComplexRect_s *)SUF_ComplexRectArrayAllocate (nRows);

    SDA_Clear ((SLData_t *) confusionMatrix,                // Array pointer
               NUM_CATEGORIES*NUM_CATEGORIES);              // Array length


    for (SLArrayIndex_t validationSequenceNumber = 0; validationSequenceNumber < nRows; validationSequenceNumber++) {
        SLData_t outputActivation;

        SLArrayIndex_t predictedCategory =
            SDA_TwoLayer2CategoryNetworkPredict (pValidationData+(validationSequenceNumber*NETWORK_INPUT_SAMPLE_LENGTH),    // Pointer to data to validate
                                                 (SLData_t *)layer1Weights,                                                 // Pointer to layer #1 weights
                                                 layer2Weights,                                                             // Pointer to layer #2 weights
                                                 pLayer1PostActivation,                                                     // Pointer to post activation for hidden layer
                                                 &outputActivation,                                                         // Pointer to output activation result
                                                 ACTIVATION_TYPE,                                                           // Hidden layer activation type
                                                 ACTIVATION_ALPHA,                                                          // Hidden layer alpha value
                                                 ACTIVATION_TYPE,                                                           // Output layer activation type
                                                 ACTIVATION_ALPHA,                                                          // Output layer alpha value
                                                 CLASSIFICATION_THRESHOLD,                                                  // Classification threshold
                                                 NETWORK_INPUT_SAMPLE_LENGTH,                                               // Input sample length
                                                 NETWORK_HIDDEN_LAYER_NODES);                                               // Hidden layer length

        totalClassificationCount++;
        if (pCategoricalValue[validationSequenceNumber] == 0) { // If category == zero
            totalActualClassZeroCount++;
        }
        else {                                              // If category == one
            totalActualClassOneCount++;
        }

        if (predictedCategory == pCategoricalValue[validationSequenceNumber]) {   // If correct detection then increment counter
            correctClassificationCount++;
        }
        else if (debugFlag == 1) {                          // If Error and we are in debug mode then print output and correct result
            printf("Correct result = %d, Classification Decision = %d, Output Activation = %lf\n",
                   (SLArrayIndex_t)pCategoricalValue[validationSequenceNumber],
                   (SLArrayIndex_t)predictedCategory,
                   outputActivation);
        }
        xyData[validationSequenceNumber].real = (SLData_t)predictedCategory;
        xyData[validationSequenceNumber].imag = outputActivation;

        confusionMatrix[pCategoricalValue[validationSequenceNumber]][predictedCategory] = confusionMatrix[(SLArrayIndex_t)pCategoricalValue[validationSequenceNumber]][predictedCategory] + SIGLIB_ONE;
    }

    printf("\n%s vs. %s\n\n", filename0, filename1);

    SLData_t TP = confusionMatrix[0][0];
    SLData_t FN = confusionMatrix[0][1];
    SLData_t FP = confusionMatrix[1][0];
    SLData_t TN = confusionMatrix[1][1];

    printf("%s vs. %s - Confusion Matrix\n", filename0, filename1);
    printf("           Predicted Category\n");
    printf("                0    1\n");
    printf("Actual     0   %d    %d\n", (int)confusionMatrix[0][0], (int)confusionMatrix[0][1]);
    printf("Category   1   %d    %d\n", (int)confusionMatrix[1][0], (int)confusionMatrix[1][1]);
    printf("\n");

    printf("%s vs. %s - Confusion Matrix (Percentage)\n", filename0, filename1);
    printf("           Predicted Category\n");
    printf("                0     1\n");
    printf("Actual     0   %d%%   %d%%\n", (int)(confusionMatrix[0][0]*SIGLIB_ONE_HUNDRED/totalClassificationCount), (int)(confusionMatrix[0][1]*SIGLIB_ONE_HUNDRED/totalClassificationCount));
    printf("Category   1   %d%%   %d%%\n", (int)(confusionMatrix[1][0]*SIGLIB_ONE_HUNDRED/totalClassificationCount), (int)(confusionMatrix[1][1]*SIGLIB_ONE_HUNDRED/totalClassificationCount));
    printf("\n");

    SLData_t predictionAccuracy  = (TP+TN) / (TP+TN+FP+FN);
    SLData_t predictionErrorRate = (FP+FN) / (TP+TN+FP+FN);
    SLData_t precision;
    SLData_t recallSensitivity;
    SLData_t specificity;
    SLData_t falseAcceptRate;
    SLData_t f1Score;

    if ((TP+FP) == SIGLIB_ZERO) {
        precision = SIGLIB_ZERO;
    }
    else {
        precision = (TP) / (TP+FP);
    }
    if ((TP+FN) == SIGLIB_ZERO) {
        recallSensitivity = SIGLIB_ZERO;
    }
    else {
        recallSensitivity = (TP) / (TP+FN);
    }
    if ((TN+FP) == SIGLIB_ZERO) {
        specificity = SIGLIB_ZERO;
        falseAcceptRate = SIGLIB_ZERO;
    }
    else {
        specificity     = (TN) / (TN+FP);
        falseAcceptRate = (FP) / (TN+FP);
    }

    if ((precision+recallSensitivity) == SIGLIB_ZERO) {
        f1Score = SIGLIB_ZERO;
    }
    else {
        // f1Score = (SIGLIB_TWO*TP) / ((SIGLIB_TWO*TP)+FP+FN);
        f1Score = SIGLIB_TWO*((precision*recallSensitivity)/(precision+recallSensitivity));
    }

    printf ("Statistics\n");
    printf ("Prediction Accuracy                              : %lf\n", predictionAccuracy);
    printf ("Prediction Error Rate (Misclassification Rate)   : %lf\n", predictionErrorRate);
    printf ("Prediction Accuracy %%                            : %lf\n", correctClassificationCount*SIGLIB_ONE_HUNDRED/nRows);
    printf ("Prediction Error Rate (Misclassification Rate) %% : %lf\n", predictionErrorRate*SIGLIB_ONE_HUNDRED/nRows);
    printf ("Precision                                        : %lf\n", precision);
    printf ("Recall Sensitivity (True Accept Rate)            : %lf\n", recallSensitivity);
    printf ("Specificity                                      : %lf\n", specificity);
    printf ("False Accept Rate  (1-Specificity)               : %lf\n", falseAcceptRate);
    printf ("F1 Score                                         : %lf\n\n", f1Score);


    if (scatterDiagramFlag == 1) {
        h_GPC_Plot  *hXYGraph;
        char        printString[100];

        printf ("Plotting Graph: nRows: %d\n", nRows);

        sprintf(printString, "True Output vs. Output Activation - Epoch count: %d", epochCount);
        hXYGraph =                                          // Initialize plot
            gpc_init_xy (printString,                       // Plot title
                         "True Output",                     // X-Axis label
                         "Output Activation",               // Y-Axis label
                         1.1,                               // Dimension - this is square
                         GPC_KEY_DISABLE);                  // Legend / key mode
        if (NULL == hXYGraph) {
            printf ("\nPlot creation failure.\n");
            exit (-1);
        }


        gpc_plot_xy (hXYGraph,                              // Graph handle
                     xyData,                                // Array of complex dataset
                     (int)nRows,                            // Dataset length
                     "Results",                             // Dataset title
                     // printString,                        // Dataset title
                     "points pt 7 ps 1",                    // Graph type
                     "red",                                 // Colour
                     GPC_NEW);                              // New graph

        printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
        gpc_close (hXYGraph);
    }

    if (confusionMatrixFlag) {
        printf ("totalActualClassZeroCount = %d\n", totalActualClassZeroCount);
        printf ("totalActualClassOneCount  = %d\n", totalActualClassOneCount);

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
    SUF_MemoryFree (pValidationData);
    SUF_MemoryFree (pCategoricalValue);
    SUF_MemoryFree (xyData);

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
                case 'm':
                    confusionMatrixFlag = 1;
                    printf("Displaying confusion matrix = True\n");
                    break;
                case 's':
                    scatterDiagramFlag = 1;
                    printf("Displaying scatter diagram = True\n");
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
    printf ("network_validate_single_category [params]\n");
    printf ("\t-0 filename   Filename #0 (Required)\n");
    printf ("\t-1 filename   Filename #1 (Required)\n");
    printf ("\t-e Number     Epoch Count\n");
    printf ("\t-m            Display confustion matrix\n");
    printf ("\t-s            Display scatter diagram\n");
    printf ("\t-d            Display debug information\n");
    printf ("\t-h            Help\n");
}

