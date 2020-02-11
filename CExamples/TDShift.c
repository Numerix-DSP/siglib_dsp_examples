// SigLib Time Domain Pitch Shifting Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define FFT_LENGTH      256
#define LOG2_FFT_LENGTH 8
#define SAMPLE_LENGTH   256
#define SHIFT_RATIO     2.3333333333333333333333333
#define SHIFT_BUF_SIZE  64

// Declare global variables and arrays
static SLData_t    pRealInput[SAMPLE_LENGTH], pRealOutput[SAMPLE_LENGTH];
static SLData_t    pImagInput[SAMPLE_LENGTH], pImagOutput[SAMPLE_LENGTH];
static SLData_t    ShiftArray[SHIFT_BUF_SIZE];

static SLData_t    pResults[SAMPLE_LENGTH];
static SLData_t    SinePhase;
static SLData_t    *pFFTCoeffs;

static SLArrayIndex_t  ShiftArrayOffsetIn;
static SLData_t    ShiftArrayOffsetOut;
static SLData_t    PreviousSample;

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    SLFixData_t i;

    ShiftArrayOffsetIn = 0;
    ShiftArrayOffsetOut = SIGLIB_ZERO;
    PreviousSample = SIGLIB_ZERO;

    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

    SinePhase = SIGLIB_ZERO;

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Time Domain Pitch Shifting",  // Plot title
                     "Time / Frequency",            // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

                                                    // Initialise FFT
    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length

    SDA_SignalGenerate (pRealInput,                 // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        0.9,                        // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.019,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    for (i = 0; i < SAMPLE_LENGTH; i++) {
        pRealOutput[i] = SDS_TdPitchShift (pRealInput[i],           // Sample
                                           ShiftArray,              // Pointer to pitch shift array
                                           &ShiftArrayOffsetIn,     // Input array offset
                                           &ShiftArrayOffsetOut,    // Output array offset
                                           &PreviousSample,         // Previous sample
                                           SHIFT_RATIO,             // Pitch shift ratio
                                           SHIFT_BUF_SIZE);         // Length of pitch shift array
    }


    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealInput,                        // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Real Input",                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nReal Input\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealOutput,                       // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Real Output",                     // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nReal Output\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Rfft (pRealInput,                           // Pointer to real array
              pImagInput,                           // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Perform real FFT
    SDA_Rfft (pRealOutput,                          // Pointer to real array
              pImagOutput,                          // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real power from complex
    SDA_LogMagnitude (pRealInput,                   // Pointer to real source array
                      pImagInput,                   // Pointer to imaginary source array
                      pResults,                     // Pointer to log magnitude destination array
                      FFT_LENGTH);                  // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Input Spectrum",                  // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nInput Spectrum\nPlease hit <Carriage Return> to continue . . ."); getchar ();


                                                    // Calculate real power from complex
    SDA_LogMagnitude (pRealOutput,                  // Pointer to real source array
                      pImagOutput,                  // Pointer to imaginary source array
                      pResults,                     // Pointer to log magnitude destination array
                      FFT_LENGTH);                  // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Output Spectrum",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nOutput Spectrum\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pFFTCoeffs);                    // Free memory
}


