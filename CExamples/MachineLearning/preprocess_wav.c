// SigLib - .wav file function read and pre-process program
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
#define FFT_LENGTH                      (NETWORK_INPUT_SAMPLE_LENGTH * 2)
#define LOG2_FFT_LENGTH                 ((SLArrayIndex_t)(SDS_Log2(FFT_LENGTH)+SIGLIB_MIN_THRESHOLD))   // Log FFT length and avoid quantization issues

#define DEFAULT_STRIDE_LENGTH           FFT_LENGTH          // Default: No data augmentation (source frame overlap)

#ifndef OUTPUT_DELAY_COUNT
#define OUTPUT_DELAY_COUNT              23                  // 23 frames at 512 samples is .25 second
#endif

#define PEAK_LEVEL_DBFS                 -13.                // Peak level dB FS
#define PEAK_LEVEL_LINEAR               (SDS_dBmToLinear(PEAK_LEVEL_DBFS, 32767.))  // Peak level dB linear

#define ONE_POLE_FILTER_ALPHA           0.9                 // One-pole filter feedback value


#define TRAINING_TO_VALIDATION_RATIO    10


static SLData_t         *pRealData, *pImagData, *pWindowCoeffs, *pFFTCoeffs, *pOnePoleState;

char    inputFilename[512];
char    inputBaseFilename[256];
char    trainingFilename[512];
char    validationFilename[512];

SLWavFileInfo_s wavInfo;


int main (int argc, char *argv[]);

int main (int argc, char *argv[])
{
    FILE            *pInputFile, *pTrainingFile, *pValidationFile;
    SLArrayIndex_t  sampleCount;
    SLArrayIndex_t  fileNumber = 0;
    SLArrayIndex_t  trainingOrValidaionSwitch = 0;
    SLArrayIndex_t  categoricalValue = 0;
    SLArrayIndex_t  copyIndexLength = 0;
    SLArrayIndex_t  copyStrideLength = DEFAULT_STRIDE_LENGTH;
    SLArrayIndex_t  overlapSrcArrayIndex;

    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pWindowCoeffs = SUF_VectorArrayAllocate (FFT_LENGTH);   // Window array
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);
    pOnePoleState = SUF_VectorArrayAllocate (NETWORK_INPUT_SAMPLE_LENGTH);

    if ((NULL == pRealData) || (NULL == pImagData) || (NULL == pWindowCoeffs) || (NULL == pFFTCoeffs)
        || (NULL == pOnePoleState)) {
        printf ("\nMemory Allocation Error\n\n");
        exit (-2);                                          // Exit - memory allocation error
    }


    if (argc == 4) {                                        // If a stride is specified then read it
        copyStrideLength = (SLArrayIndex_t)atol (argv[3]);
    }

    else if (argc != 3) {
        printf ("\nUsage error  :\npreprocess_wav filename (no extension) categoricalValue [stride]\n");
        printf ("The categorical value is used to place the processed files into the provided category\n");
        printf ("The stride value is an optional parameter used to include an overslap stride when reading the source data\n\n");
        exit (-1);                                          // Exit - usage error
    }

    strcpy (inputBaseFilename, argv[1]);
#if defined (_MSC_VER)                                      // Defined by Microsoft compilers
    strcpy (inputFilename, "Original\\");
#else
    strcpy (inputFilename, "Original/");
#endif
    strcat (inputFilename, argv[1]);
    strcat (inputFilename, ".wav");

    categoricalValue = (SLArrayIndex_t)atol (argv[2]);

    printf("\nPre-processing The Data ...\n");
    printf ("Source file       : %s\n", inputFilename);
    printf ("inputBaseFilename : %s\n", inputBaseFilename);
    printf ("Categorical Value : %d\n", categoricalValue);
    printf ("Copy stride length: %d\n", copyStrideLength);

    if ((pInputFile = fopen(inputFilename, "rb")) == NULL) { // Note this file is binary
        printf ("Error opening input file %s\n", inputFilename);
        exit (-1);
    }

#if defined (_MSC_VER)                                      // Defined by Microsoft compilers
    strcpy (trainingFilename, "PreProcessed\\Training_");
#else
    strcpy (trainingFilename, "PreProcessed/Training_");
#endif
    strcat (trainingFilename, inputBaseFilename);
    strcat (trainingFilename, "_");
    strcat (trainingFilename, argv[2]);
    strcat (trainingFilename, ".csv");

#if defined (_MSC_VER)                                      // Defined by Microsoft compilers
    strcpy (validationFilename, "PreProcessed\\Validation_");
#else
    strcpy (validationFilename, "PreProcessed/Validation_");
#endif
    strcat (validationFilename, inputBaseFilename);
    strcat (validationFilename, "_");
    strcat (validationFilename, argv[2]);
    strcat (validationFilename, ".csv");

    // printf ("wp: trainingFilename = %s\n", trainingFilename);
    // printf ("wp: validationFilename = %s\n", validationFilename);

    if ((pTrainingFile = fopen(trainingFilename, "a")) == NULL) {   // Open spreadsheets in append mode
        printf ("Error opening training spreadsheet: %s\n", trainingFilename);
        exit (-1);
    }

    if ((pValidationFile = fopen(validationFilename, "a")) == NULL) {
        printf ("Error opening validation spreadsheet: %s\n", validationFilename);
        exit (-1);
    }


    wavInfo = SUF_WavReadHeader (pInputFile);
    SUF_WavDisplayInfo (wavInfo);
    if (wavInfo.NumberOfChannels != 1) {                    // Check how many channels
        printf ("Number of channels in %s = %d\n", inputFilename, wavInfo.NumberOfChannels);
        printf ("This app requires a mono .wav file\n");
        exit (-1);
    }

    SIF_CopyWithIndex (&overlapSrcArrayIndex);              // Pointer to source array index

                                                            // Initialise FFT
    SIF_Fft (pFFTCoeffs,                                    // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,                   // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                                   // FFT length

    SIF_Window (pWindowCoeffs,                              // Pointer to window oefficient
                SIGLIB_HANNING,                             // Window type
                SIGLIB_ZERO,                                // Window coefficient
                FFT_LENGTH);                                // Window length

    SDA_Clear (pOnePoleState,                               // Pointer to array
               NETWORK_INPUT_SAMPLE_LENGTH);                // Array length


    SLArrayIndex_t globalSampleCount = 0;
    SLData_t globalPeak = SIGLIB_ZERO;

                                                            // Extract the peak level to normalize all levels
    while ((sampleCount = SUF_WavReadData (pRealData, pInputFile, wavInfo, FFT_LENGTH)) == FFT_LENGTH) {
        globalSampleCount += sampleCount;

        SLData_t localPeak = SDA_Max (pRealData,            // Source array pointer
                                      FFT_LENGTH);          // Array length
        if (localPeak > globalPeak) {
            globalPeak = localPeak;
        }
    }
    rewind (pInputFile);                                    // Rewind the file and process the data
    wavInfo = SUF_WavReadHeader (pInputFile);

    SLData_t gainFactor = PEAK_LEVEL_LINEAR / globalPeak;   // Calculate the scaling factor
    printf ("Gain factor = %lf\n", gainFactor);

    SLData_t *pInputArray = SUF_VectorArrayAllocate (globalSampleCount);    // Input data array

    if (NULL == pInputArray) {
        printf ("\nMemory Allocation Error - pInputArray\n\n");
        exit (-2);                                          // Exit - memory allocation error
    }


    sampleCount =
        SUF_WavReadData (pInputArray, pInputFile, wavInfo, globalSampleCount);

    do {
        copyIndexLength =
            SDA_CopyWithIndex (pInputArray,                 // Pointer to source array
                               pRealData,                   // Pointer to destination array
                               &overlapSrcArrayIndex,       // Pointer to source array index
                               globalSampleCount,           // Source array length
                               copyStrideLength,            // Stride length
                               FFT_LENGTH);                 // Destination array length

        if (copyIndexLength == FFT_LENGTH) {                // If we have a full array to process then process it
                                                            // Remove any D.C. offset
            SDA_SubtractMean (pRealData,                    // Source array pointer
                              pRealData,                    // Destination array pointer
                              SIGLIB_ONE / FFT_LENGTH,      // Inverse of array length
                              FFT_LENGTH);                  // Array length

            SDA_Multiply (pRealData,                        // Source array pointer
                          gainFactor,                       // Maximum scaled value
                          pRealData,                        // Destination array pointer
                          FFT_LENGTH);                      // Array length

                                                            // Apply window to data
            SDA_Window (pRealData,                          // Pointer to source array
                        pRealData,                          // Pointer to destination array
                        pWindowCoeffs,                      // Pointer to window coefficients
                        FFT_LENGTH);                        // Window length

                                                            // Perform real FFT
            SDA_Rfft (pRealData,                            // Pointer to real array
                      pImagData,                            // Pointer to imaginary array
                      pFFTCoeffs,                           // Pointer to FFT coefficients
                      SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
                      FFT_LENGTH,                           // FFT length
                      LOG2_FFT_LENGTH);                     // log2 FFT length

                                                                // Calculate real power from complex
            SDA_LogMagnitude (pRealData,                        // Pointer to real source array
                              pImagData,                        // Pointer to imaginary source array
                              pRealData,                        // Pointer to log magnitude destination array
                              NETWORK_INPUT_SAMPLE_LENGTH);     // Array length

            SDA_OnePolePerSample (pRealData,                    // Data to be filtered
                                  pRealData,                    // Filtered output array
                                  pOnePoleState,                // State array
                                  ONE_POLE_FILTER_ALPHA,        // Feedback alpha
                                  NETWORK_INPUT_SAMPLE_LENGTH); // Array lengths

            if (trainingOrValidaionSwitch <= TRAINING_TO_VALIDATION_RATIO) { // Store results in training or validation spreadsheet
                for (int i = 0; i < NETWORK_INPUT_SAMPLE_LENGTH; i++) { // Write to training spreadsheet
                    fprintf (pTrainingFile, "%lf,", pRealData[i]);
                }
                fprintf (pTrainingFile, "%d\n", categoricalValue);
            }
            else {
                for (int i = 0; i < NETWORK_INPUT_SAMPLE_LENGTH; i++) { // Write to validation spreadsheet
                    fprintf (pValidationFile, "%lf,", pRealData[i]);
                }
                fprintf (pValidationFile, "%d\n", categoricalValue);

                trainingOrValidaionSwitch = 0;              // Reset switch
            }
            trainingOrValidaionSwitch++;

            fileNumber++;
        }
    } while (copyIndexLength == FFT_LENGTH);

    fclose (pInputFile);                                    // Close the files
    fclose (pTrainingFile);
    fclose (pValidationFile);

    free (pRealData);                                       // Free memory
    free (pImagData);
    free (pWindowCoeffs);
    free (pFFTCoeffs);
    free (pOnePoleState);

    return fileNumber;
}

