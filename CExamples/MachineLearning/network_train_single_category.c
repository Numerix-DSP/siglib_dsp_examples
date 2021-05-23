// SigLib Signal Neural Network Training Program
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
#define NETWORK_INPUT_SAMPLE_LENGTH     128
#endif
#ifndef NETWORK_HIDDEN_LAYER_NODES
#define NETWORK_HIDDEN_LAYER_NODES      25
#endif

#define NUM_LAYERS                      2                   // Number of layers

#define LEARNING_RATE                   SIGLIB_ONE

#define DEFAULT_EPOCH_COUNT             5

#define ENABLE_ADDITIONAL_QUANTIZATION  0                   // Set to '1' to enable additional weight quantization, for testing
#define QUANTIZATION_NUMBER_OF_BITS     6                   // Number of bits

// Declare global variables and arrays
static SLArrayIndex_t   *pCategoricalValue;
static SLData_t         *pShuffledTrainingData;

static SLData_t         *pLayer1PreActivation, *pLayer1PostActivation;

static char             filename0[512];
static char             filename1[512];
static char             filenameWeights[1200];

static SLData_t         shuffleRowTmp[NETWORK_INPUT_SAMPLE_LENGTH+1];   // Remember line length = NETWORK_INPUT_SAMPLE_LENGTH+1, for the 'mode' column

static SLData_t         layer1Weights[NETWORK_HIDDEN_LAYER_NODES][NETWORK_INPUT_SAMPLE_LENGTH];   // Weights for each layer
static SLData_t         layer2Weights[NETWORK_HIDDEN_LAYER_NODES];


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


int         epochCount = DEFAULT_EPOCH_COUNT;
int         debugFlag = 0;

void show_help (void);
void parse_command_line (int argc, char *argv[]);

int main (int argc, char *argv[])
{
    pLayer1PreActivation = SUF_VectorArrayAllocate (NETWORK_HIDDEN_LAYER_NODES);   // Allocate arrays
    pLayer1PostActivation = SUF_VectorArrayAllocate (NETWORK_HIDDEN_LAYER_NODES);

    if ((NULL == pLayer1PreActivation) || (NULL == pLayer1PostActivation)) {
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


    printf("\nTraining The Network ...\n");
    printf("    Filename #0    : %s\n", filename0);
    printf("    Filename #1    : %s\n", filename1);
    printf("    Epoch count    : %d\n", epochCount);
    printf("    Activation Type: %d\n\n", (int)ACTIVATION_TYPE);


    SDA_SignalGenerate ((SLData_t *)layer1Weights,                                  // Pointer to destination array
                        SIGLIB_WHITE_NOISE,                                         // Signal type - random white noise
                        SIGLIB_ONE,                                                 // Signal peak level
                        SIGLIB_FILL,                                                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                                                // Signal frequency - Unused
                        SIGLIB_ZERO,                                                // D.C. Offset
                        SIGLIB_ZERO,                                                // Unused
                        SIGLIB_ZERO,                                                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,                                       // Unused
                        SIGLIB_NULL_DATA_PTR,                                       // Unused
                        NETWORK_INPUT_SAMPLE_LENGTH*NETWORK_HIDDEN_LAYER_NODES);    // Dataset length

    SDA_SignalGenerate (layer2Weights,                      // Pointer to destination array
                        SIGLIB_WHITE_NOISE,                 // Signal type - random white noise
                        SIGLIB_ONE,                         // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                        // Signal frequency - Unused
                        SIGLIB_ZERO,                        // D.C. Offset
                        SIGLIB_ZERO,                        // Unused
                        SIGLIB_ZERO,                        // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,               // Unused
                        SIGLIB_NULL_DATA_PTR,               // Unused
                        NETWORK_HIDDEN_LAYER_NODES);        // Dataset length


    SDA_Fill (pLayer1PreActivation, SIGLIB_ZERO, NETWORK_HIDDEN_LAYER_NODES);   // Fill with zeros
    SDA_Fill (pLayer1PostActivation, SIGLIB_ZERO, NETWORK_HIDDEN_LAYER_NODES);

                                            // Load training data
    SLArrayIndex_t nRows, nCols;
    SLData_t *pTrainingData = NULL;                         // Pointer only - memory will be allocated in SUF_CsvReadMatrix()
    SUF_CsvReadMatrix (&pTrainingData, "Tdata.csv", SIGLIB_FIRST_ROW_KEEP, &nRows, &nCols);

    if ((nRows == 0) || (NULL == pTrainingData)) {
        printf ("\n\nSUF_CsvReadMatrix (Tdata.csv) failure!.\n\n");
        exit (-1);
    }

    if ((nCols-1) != NETWORK_INPUT_SAMPLE_LENGTH) {
        printf ("Error - Training sequences must be %d samples long\n", NETWORK_INPUT_SAMPLE_LENGTH);
        printf ("        Provided sequence was %d samples long\n", nCols);
        exit(-1);
    }

    if (debugFlag == 1) {
        printf("nCols = %d, NETWORK_INPUT_SAMPLE_LENGTH = %d\n", nCols-1, NETWORK_INPUT_SAMPLE_LENGTH);

        printf("Number of training sequences:   %d\n", nRows);
        printf ("Training Data\n");
        SUF_PrintMatrix (pTrainingData, nRows, NETWORK_INPUT_SAMPLE_LENGTH);
    }

    pCategoricalValue = SUF_IndexArrayAllocate(nRows);               // Allocate the categorical value array
    pShuffledTrainingData =  SUF_VectorArrayAllocate(nRows*nCols);   // Allocate the shuffled data array
    if ((NULL == pCategoricalValue) || (NULL == pShuffledTrainingData)) {
        printf ("\n\nMemory allocation failed (pCategoricalValue and pShuffledTrainingData)\n\n");
        exit (-1);
    }

    for (SLArrayIndex_t e = 0; e < epochCount; e++) {
                                                            // Randomly shuffle the rows in the training data frame
        SMX_ShuffleRows (pTrainingData, pShuffledTrainingData, shuffleRowTmp, nRows, nCols);

        SMX_ExtractCategoricalColumn (pShuffledTrainingData,// Pointer to source matrix
                                      pCategoricalValue,    // Pointer to destination array
                                      nRows,                // Number of rows in matrix
                                      nCols);               // Number of columns in matrix
        SMX_DeleteOldColumn (pShuffledTrainingData,         // Pointer to source matrix
                             pShuffledTrainingData,         // Pointer to destination matrix
                             nCols-1,                       // Column number to delete
                             nRows,                         // Number of rows in matrix
                             nCols);                        // Number of columns in matrix

        SLArrayIndex_t nInputNodes = nCols-1;               // We have removed the categorical value column

                                                            // Scale all samples to avoid overflow in exponent function
        SDA_Multiply (pShuffledTrainingData,                // Pointer to source array
                      SIGLIB_ONE/MAGNITUDE_SCALING_FACTOR,  // Scalar value
                      pShuffledTrainingData,                // Pointer to destination array
                      nRows*nInputNodes);                   // Array length

                                                            // Train the network
        SDA_TwoLayer2CategoryNetworkFit (pShuffledTrainingData,         // Pointer to training data
                                         pCategoricalValue,             // Pointer to training signal type
                                         (SLData_t *)layer1Weights,     // Pointer to layer #1 weights
                                         layer2Weights,                 // Pointer to layer #2 weights
                                         pLayer1PreActivation,          // Pointer to pre activation for hidden layer
                                         pLayer1PostActivation,         // Pointer to post activation for hidden layer
                                         ACTIVATION_TYPE,               // Hidden layer activation type
                                         ACTIVATION_ALPHA,              // Hidden layer alpha value
                                         ACTIVATION_TYPE,               // Output layer activation type
                                         ACTIVATION_ALPHA,              // Output layer alpha value
                                         LEARNING_RATE,                 // Learning rate
                                         nRows,                         // Number of training sequences
                                         NETWORK_INPUT_SAMPLE_LENGTH,   // Input sample length
                                         NETWORK_HIDDEN_LAYER_NODES);   // Hidden layer length
    }

                                                            // Quantize the weights
    if (debugFlag == 1) {
        // printf("layer1Weights\n"); SUF_PrintMatrix((SLData_t *)layer1Weights, NETWORK_HIDDEN_LAYER_NODES, NETWORK_INPUT_SAMPLE_LENGTH);
        // printf("layer2Weights\n"); SUF_PrintMatrix((SLData_t *)layer2Weights, NUM_CATEGORIES, NETWORK_HIDDEN_LAYER_NODES);

        printf ("Peak layer1Weights: %lf\n", SDA_Max((SLData_t *)layer1Weights, NETWORK_HIDDEN_LAYER_NODES*NETWORK_INPUT_SAMPLE_LENGTH));   // Peak Value = 2.0
        printf ("Peak layer2Weights: %lf\n", SDA_Max(layer2Weights, NETWORK_HIDDEN_LAYER_NODES));                           // Peak Value = 4.0
    }

#if ENABLE_ADDITIONAL_QUANTIZATION
                            // Can apply additional quantization for testing
    SDA_Quantize((SLData_t *)layer1Weights,                                 // Source array pointer
                 (SLData_t *)layer1Weights,                                 // Destination array pointer
                 QUANTIZATION_NUMBER_OF_BITS,                               // Quantisation number of bits
                 LAYER_1_PEAK,                                              // Peak input value
                 NETWORK_HIDDEN_LAYER_NODES*NETWORK_INPUT_SAMPLE_LENGTH);   // Array length
    SDA_Quantize(layer2Weights,                                     // Source array pointer
                 layer2Weights,                                     // Destination array pointer
                 QUANTIZATION_NUMBER_OF_BITS,                       // Quantisation number of bits
                 LAYER_2_PEAK,                                      // Peak input value
                 NETWORK_HIDDEN_LAYER_NODES);                       // Array length
#endif

                            // Write weights to file : Binary plus C header : float and 8 bit words
#if defined (_MSC_VER)                                      // Defined by Microsoft compilers
    sprintf (filenameWeights, "weightCoefficientsFiles\\weights%s_%s.h", filename0, filename1);
#else
    sprintf (filenameWeights, "weightCoefficientsFiles/weights%s_%s.h", filename0, filename1);
#endif
    SUF_WriteWeightsIntegerCFile (filenameWeights,                                          // Filename
                                  (SLData_t *)layer1Weights,                                // layer1Weights
                                  (SLData_t *)layer2Weights,                                // layer2Weights
                                  NETWORK_HIDDEN_LAYER_NODES*NETWORK_INPUT_SAMPLE_LENGTH,   // layer1Weights array length
                                  NETWORK_HIDDEN_LAYER_NODES,                               // layer2Weights array length
                                  NUM_LAYERS);                                              // Number of layers

#if defined (_MSC_VER)                                      // Defined by Microsoft compilers
    sprintf (filenameWeights, "weightCoefficientsFiles\\weightsFloat%s_%s.h", filename0, filename1);
#else
    sprintf (filenameWeights, "weightCoefficientsFiles/weightsFloat%s_%s.h", filename0, filename1);
#endif
    SUF_WriteWeightsFloatCFile (filenameWeights,                                        // Filename
                                (SLData_t *)layer1Weights,                              // layer1Weights
                                (SLData_t *)layer2Weights,                              // layer2Weights
                                NETWORK_HIDDEN_LAYER_NODES*NETWORK_INPUT_SAMPLE_LENGTH, // layer1Weights array length
                                NETWORK_HIDDEN_LAYER_NODES,                             // layer2Weights array length
                                NUM_LAYERS);                                            // Number of layers

#if defined (_MSC_VER)                                      // Defined by Microsoft compilers
    sprintf (filenameWeights, "weightCoefficientsFiles\\weights%s_%s.dat", filename0, filename1);
#else
    sprintf (filenameWeights, "weightCoefficientsFiles/weights%s_%s.dat", filename0, filename1);
#endif

    SUF_WriteWeightsBinaryFile (filenameWeights,                                        // Filename
                                (SLData_t *)layer1Weights,                              // layer1Weights
                                (SLData_t *)layer2Weights,                              // layer2Weights
                                NETWORK_HIDDEN_LAYER_NODES*NETWORK_INPUT_SAMPLE_LENGTH, // layer1Weights array length
                                NETWORK_HIDDEN_LAYER_NODES,                             // layer2Weights array length
                                NUM_LAYERS);                                            // Number of layers

    SUF_MemoryFree (pLayer1PreActivation);                  // Free memory
    SUF_MemoryFree (pLayer1PostActivation);
    SUF_MemoryFree (pTrainingData);
    SUF_MemoryFree (pShuffledTrainingData);
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
    printf ("network_train_single_category [params]\n");
    printf ("\t-0 filename   Filename #0 (Required)\n");
    printf ("\t-1 filename   Filename #1 (Required)\n");
    printf ("\t-e Number     Epoch Count\n");
    printf ("\t-d            Display debug information\n");
    printf ("\t-h            Help\n");
}

