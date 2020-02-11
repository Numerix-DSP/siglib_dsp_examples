// Spectral Inversion Of A Chirp Signal Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH   512
#define FFT_LENGTH      512
#define LOG2_FFT_LENGTH 9

// Declare global variables and arrays
static SLData_t    *pRealData, *pImagData, *pResults, *pInverse, *pFFTCoeffs;
static SLData_t    ChirpPhase, ChirpValue;

void    main(void);

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    pRealData = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pInverse = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);


    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Spectral Inversion",          // Plot title
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

    ChirpPhase = SIGLIB_ZERO;
    ChirpValue = SIGLIB_ZERO;

    SDA_SignalGenerate (pRealData,                  // Pointer to destination array
                        SIGLIB_CHIRP_NL,            // Signal type - Chirp with non linear frequency ramp
                        0.45,                       // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.0025,                     // Signal lower frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        1.007,                      // Frequency change per sample period
                        SIGLIB_HALF,                // Signal upper frequency
                        &ChirpPhase,                // Chirp phase - used for next iteration
                        &ChirpValue ,               // Chirp current value - used for next iteration
                        SAMPLE_LENGTH);             // Output dataset length

    SDA_RealSpectralInverse (pRealData,             // Source array
                             pInverse,              // Destination array
                             SAMPLE_LENGTH);        // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Chirp Signal",                    // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nChirp Signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();


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


    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Chirp Spectrum",                  // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nChirp Spectrum\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pInverse,                          // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Spectrally Inverted Chirp Signal", // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSpectrally Inverted Chirp Signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();


                                                    // Perform real FFT
    SDA_Rfft (pInverse,                             // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real magnitude from complex
    SDA_Magnitude (pInverse,                        // Pointer to real source array
                   pImagData,                       // Pointer to imaginary source array
                   pResults,                        // Pointer to magnitude destination array
                   FFT_LENGTH);                     // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Spectrally Inverted Chirp Spectrum",   // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSpectrally Inverted Chirp Spectrum\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pRealData);                     // Free memory
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pInverse);
    SUF_MemoryFree (pResults);
    SUF_MemoryFree (pFFTCoeffs);
}

