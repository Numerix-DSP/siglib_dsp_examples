// Example : Spectrum Of A Sine Wave and the same wave, with a single sample missing, near the centre
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define FFT_LENGTH      512
#define HALF_FFT_LENGTH 256
#define LOG2_FFT_LENGTH 9

// Declare global variables and arrays
static SLData_t     *pRealData, *pImagData, *pResults, *pFFTCoeffs;
static SLData_t     SinePhase;

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    SLData_t    max;
                                                    // Allocate memory
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

                // RMS result array
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Sine Wave Fourier Transform", // Plot title
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

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        0.9,                        // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.019,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        FFT_LENGTH);                // Output dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Original Sine Wave",              // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nOriginal Sine Wave\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Rfft (pRealData,                            // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real magnitude from complex
    SDA_Magnitude (pRealData,                       // Pointer to real source array
                   pImagData,                       // Pointer to imaginary source array
                   pResults,                        // Pointer to magnitude destination array
                   FFT_LENGTH);                     // Dataset length
    SDA_20Log10 (pResults,                          // Pointer to source array
                 pResults,                          // Pointer to destination array
                 FFT_LENGTH);                       // Dataset length
    max = SDA_AbsMax (pResults,                     // Pointer to source array
                      FFT_LENGTH);                  // Dataset length
    SDA_Offset (pResults,                           // Pointer to source array
                max,                                // D.C. offset
                pResults,                           // Pointer to destination array
                FFT_LENGTH);                        // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Original Sine Wave Spectrum",     // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nOriginal Sine Wave Spectrum\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    SDA_SignalGenerate (pRealData,                          // Pointer to destination array
                        SIGLIB_SINE_WAVE,                   // Signal type - Sine wave
                        0.9,                                // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        SIGLIB_ONE / 512.,                  // Signal frequency
                        SIGLIB_ZERO,                        // D.C. Offset
                        SIGLIB_ZERO,                        // Unused
                        SIGLIB_ZERO,                        // Signal end value - Unused
                        &SinePhase,                         // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,               // Unused
                        (FFT_LENGTH /2) + 3);               // Output dataset length

    SDA_SignalGenerate ((pRealData + (FFT_LENGTH /2) + 2),  // Pointer to destination array
                        SIGLIB_SINE_WAVE,                   // Signal type - Sine wave
                        0.9,                                // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        SIGLIB_ONE / 512.,                  // Signal frequency
                        SIGLIB_ZERO,                        // D.C. Offset
                        SIGLIB_ZERO,                        // Unused
                        SIGLIB_ZERO,                        // Signal end value - Unused
                        &SinePhase,                         // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,               // Unused
                        (FFT_LENGTH /2) - 2);               // Output dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Distorted Sine Wave",             // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nDistorted Sine Wave Spectrum\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Rfft (pRealData,                            // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real magnitude from complex
    SDA_Magnitude (pRealData,                       // Pointer to real source array
                   pImagData,                       // Pointer to imaginary source array
                   pResults,                        // Pointer to magnitude destination array
                   FFT_LENGTH);                     // Dataset length
    SDA_20Log10 (pResults,                          // Pointer to source array
                 pResults,                          // Pointer to destination array
                 FFT_LENGTH);                       // Dataset length
    max = SDA_AbsMax (pResults,                     // Pointer to source array
                      FFT_LENGTH);                  // Dataset length
    SDA_Offset (pResults,                           // Pointer to source array
                max,                                // D.C. offset
                pResults,                           // Pointer to destination array
                FFT_LENGTH);                        // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Distorted Sine Wave Spectrum",    // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nDistorted Sine Wave Spectrum\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);
}


