// SigLib FFT and Hanning window Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                         // SigLib DSP library
#include <gnuplot_c.h>                                      // Gnuplot/C

// Define constants
#define FFT_LENGTH      512
#define LOG2_FFT_LENGTH ((SLArrayIndex_t)(SDS_Log2(FFT_LENGTH)+SIGLIB_MIN_THRESHOLD))   // Log FFT length and avoid quantization issues
#define WINDOW_LENGTH   FFT_LENGTH

// Declare global variables and arrays
static SLData_t         *pRealData, *pImagData, *pRealDataCopy, *pImagDataCopy, *pWindowCoeffs, *pResults, *pFFTCoeffs;
static SLData_t         SinePhase;

void main(void)
{
    h_GPC_Plot  *h2DPlot;                                   // Plot object

                                                            // Allocate memory
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pRealDataCopy = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagDataCopy = SUF_VectorArrayAllocate (FFT_LENGTH);
    pWindowCoeffs = SUF_VectorArrayAllocate (WINDOW_LENGTH);// Window array
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);        // RMS result array

    if ((NULL == pRealData) || (NULL == pImagData) || (NULL == pRealDataCopy) || (NULL == pImagDataCopy) ||
        (NULL == pWindowCoeffs) || (NULL == pFFTCoeffs) || (NULL == pResults)) {
        printf ("\n\nMemory allocation failed\n\n");
        exit (0);
    }

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                          // Pointer to destination array
                        SIGLIB_SINE_WAVE,                   // Signal type - Sine wave
                        0.9,                                // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        0.015625,                           // Signal frequency
                        SIGLIB_ZERO,                        // D.C. Offset
                        SIGLIB_ZERO,                        // Unused
                        SIGLIB_ZERO,                        // Signal end value - Unused
                        &SinePhase,                         // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR ,              // Unused
                        FFT_LENGTH);                        // Output dataset length

    h2DPlot =                                               // Initialize plot
        gpc_init_2d ("Fast Fourier Transform",              // Plot title
                     "Time",                                // X-Axis label
                     "Magnitude",                           // Y-Axis label
                     GPC_AUTO_SCALE,                        // Scaling mode
                     GPC_SIGNED,                            // Sign mode
                     GPC_KEY_ENABLE);                       // Legend / key mode
    if (NULL == h2DPlot) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

                                                            // Initialise FFT
    SIF_Fft (pFFTCoeffs,                                    // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,                   // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                                   // FFT length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pRealData,                                 // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "Synthesized sine wave",                   // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "magenta",                                 // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nSynthesized sine wave\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                            // Perform real FFT
    SDA_Rfft (pRealData,                                    // Pointer to real array
              pImagData,                                    // Pointer to imaginary array
              pFFTCoeffs,                                   // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,                  // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                                   // FFT length
              LOG2_FFT_LENGTH);                             // log2 FFT length

    SDA_Copy (pRealData,                                    // Pointer to source array
              pRealDataCopy,                                // Pointer to destination array
              FFT_LENGTH);                                  // Dataset length
    SDA_Copy (pImagData,                                    // Pointer to source array
              pImagDataCopy,                                // Pointer to destination array
              FFT_LENGTH);                                  // Dataset length

                                                            // Calculate real power from complex
    SDA_LogMagnitude (pRealData,                            // Pointer to real source array
                      pImagData,                            // Pointer to imaginary source array
                      pResults,                             // Pointer to log magnitude destination array
                      FFT_LENGTH);                          // Dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pResults,                                  // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "FFT of pure sine wave (integer number of bins)",   // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "blue",                                    // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nReal FFT of pure sine wave (integer number of bins)\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                          // Pointer to destination array
                        SIGLIB_SINE_WAVE,                   // Signal type - Sine wave
                        0.9,                                // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        0.019,                              // Signal frequency
                        SIGLIB_ZERO,                        // D.C. Offset
                        SIGLIB_ZERO,                        // Unused
                        SIGLIB_ZERO,                        // Signal end value - Unused
                        &SinePhase,                         // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR ,              // Unused
                        FFT_LENGTH);                        // Output dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pRealData,                                 // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "Synthesized sine wave",                   // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "magenta",                                 // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nSynthesized sine wave\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                            // Perform real FFT
    SDA_Rfft (pRealData,                                    // Pointer to real array
              pImagData,                                    // Pointer to imaginary array
              pFFTCoeffs,                                   // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,                  // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                                   // FFT length
              LOG2_FFT_LENGTH);                             // log2 FFT length

    SDA_Copy (pRealData,                                    // Pointer to source array
              pRealDataCopy,                                // Pointer to destination array
              FFT_LENGTH);                                  // Dataset length
    SDA_Copy (pImagData,                                    // Pointer to source array
              pImagDataCopy,                                // Pointer to destination array
              FFT_LENGTH);                                  // Dataset length

                                                            // Calculate real power from complex
    SDA_LogMagnitude (pRealData,                            // Pointer to real source array
                      pImagData,                            // Pointer to imaginary source array
                      pResults,                             // Pointer to log magnitude destination array
                      FFT_LENGTH);                          // Dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pResults,                                  // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "FFT of pure sine wave (integer number of bins)",   // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "blue",                                    // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nReal FFT of pure sine wave (integer number of bins)\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                          // Pointer to destination array
                        SIGLIB_COS_WAVE,                    // Signal type - Cosine wave
                        0.9,                                // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        0.0078125,                          // Signal frequency
                        SIGLIB_ZERO,                        // D.C. Offset
                        SIGLIB_ZERO,                        // Unused
                        SIGLIB_ZERO,                        // Signal end value - Unused
                        &SinePhase,                         // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR ,              // Unused
                        FFT_LENGTH);                        // Output dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pRealData,                                 // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "Hanning windowed sine wave",              // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "magenta",                                 // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nHanning windowed sine wave\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                            // Generate Hanning window table
    SIF_Window (pWindowCoeffs,                              // Pointer to window oefficient
                SIGLIB_HANNING,                             // Window type
                SIGLIB_ZERO,                                // Window coefficient
                FFT_LENGTH);                                // Window length
                                                            // Apply window to real data
                                                            // Apply window to data
    SDA_Window (pRealData,                                  // Pointer to source array
                pRealData,                                  // Pointer to destination array
                pWindowCoeffs,                              // Pointer to window coefficients
                WINDOW_LENGTH);                             // Window length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pRealData,                                 // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "Hanning windowed sine wave",              // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "magenta",                                 // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nHanning windowed sine wave\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                            // Perform real FFT
    SDA_Rfft (pRealData,                                    // Pointer to real array
              pImagData,                                    // Pointer to imaginary array
              pFFTCoeffs,                                   // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,                  // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                                   // FFT length
              LOG2_FFT_LENGTH);                             // log2 FFT length

    SDA_Copy (pRealData,                                    // Pointer to source array
              pRealDataCopy,                                // Pointer to destination array
              FFT_LENGTH);                                  // Dataset length
    SDA_Copy (pImagData,                                    // Pointer to source array
              pImagDataCopy,                                // Pointer to destination array
              FFT_LENGTH);                                  // Dataset length

                                                            // Shift D.C. location
    SDA_CfftShift (pRealData,                               // Pointer to real array
                   pImagData,                               // Pointer to imaginary array
                   pRealData,                               // Pointer to real array
                   pImagData,                               // Pointer to imaginary array
                   FFT_LENGTH);                             // FFT length

                                                            // Calculate real power from complex
    SDA_LogMagnitude (pRealData,                            // Pointer to real source array
                      pImagData,                            // Pointer to imaginary source array
                      pResults,                             // Pointer to log magnitude destination array
                      FFT_LENGTH);                          // Dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pResults,                                  // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "FFT of windowed sine wave (dB), non integer # of bins, D.C. Shifted", // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "blue",                                    // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nReal FFT of windowed sine wave (dB), non integer # of bins, D.C. Shifted\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    SDA_Cifft (pRealDataCopy, pImagDataCopy, pFFTCoeffs, SIGLIB_NULL_ARRAY_INDEX_PTR, FFT_LENGTH, LOG2_FFT_LENGTH); // Perform inverse FFT

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pRealDataCopy,                             // Dataset
                 FFT_LENGTH,                                // Dataset length
                 "IFFT done",                               // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 (double)(FFT_LENGTH - 1),                  // Maximum X value
                 "lines",                                   // Graph type
                 "magenta",                                 // Colour
                 GPC_NEW);                                  // New graph
    printf ("\nComplex IFFT done\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pRealData);                             // Free memory
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pRealDataCopy);
    SUF_MemoryFree (pImagDataCopy);
    SUF_MemoryFree (pWindowCoeffs);
    SUF_MemoryFree (pFFTCoeffs);
    SUF_MemoryFree (pResults);
}


