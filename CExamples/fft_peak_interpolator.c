// SigLib FFT peak estimator using quadratic interpolation
// This example plots the error for a range of input frequencies
// Max gain error percentage = 4.519154, max frequency error percentage = 0.596755
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C


#define NUM_FREQS   41
#define SOURCE_MAGN SIGLIB_ONE
#define FREQ_START  0.013671875                     // Bin 7
#define FREQ_END    0.017578125                     // Bin 9
#define FREQ_INC    (FREQ_END - FREQ_START) / (NUM_FREQS - 1)   // Frequency increment

// Define constants
#define FFT_LENGTH          512
#define LOG2_FFT_LENGTH     9
#define FFT_HALF_LENGTH     (FFT_LENGTH>>1)
#define WINDOW_SIZE         FFT_LENGTH

// Declare global variables and arrays
static SLData_t         *pRealData, *pImagData, *pWindowCoeffs, *pFFTCoeffs, *pResults;
static SLData_t         SinePhase;


void main (void)
{
    h_GPC_Plot  *h2DPlot;                               // Plot objects

    SLData_t    centerFreq;

    SLData_t    magnErrorPercent[NUM_FREQS], freqErrorPercent[NUM_FREQS];

                                                    // Allocate memory
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pWindowCoeffs = SUF_VectorArrayAllocate (WINDOW_SIZE);  // Window array
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);        // RMS result array

    h2DPlot =                                               // Initialize plot
        gpc_init_2d ("FFT Peak Interpolation",              // Plot title
                     "Frequency",                           // X-Axis label
                     "Magnitude / Frequency Percentage Error",  // Y-Axis label
                     GPC_AUTO_SCALE,                        // Scaling mode
                     GPC_SIGNED,                            // Sign mode
                     GPC_KEY_ENABLE);                       // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

                                                        // Generate Hanning window table
    SIF_Window (pWindowCoeffs,                          // Pointer to window oefficient
                SIGLIB_BLACKMAN,                        // Window type
                SIGLIB_ZERO,                            // Window coefficient
                FFT_LENGTH);                            // Window length

    SLData_t windowInverseCoherentGain =
        SDA_WindowInverseCoherentGain (pWindowCoeffs,   // Pointer to window oefficient
                                       FFT_LENGTH);     // Window length

                                                        // Initialise FFT
    SIF_Fft (pFFTCoeffs,                                // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,               // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                               // FFT length

    centerFreq = FREQ_START;
    for (int i = 0; i < NUM_FREQS; i++, centerFreq += FREQ_INC) {
        SinePhase = SIGLIB_ZERO;
        SDA_SignalGenerate (pRealData,                  // Pointer to destination array
                            SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                            SOURCE_MAGN,                // Signal peak level
                            SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                            centerFreq,                 // Signal frequency
                            SIGLIB_ZERO,                // D.C. Offset
                            SIGLIB_ZERO,                // Unused
                            SIGLIB_ZERO,                // Signal end value - Unused
                            &SinePhase,                 // Signal phase - maintained across array boundaries
                            SIGLIB_NULL_DATA_PTR ,      // Unused
                            FFT_LENGTH);                // Output dataset length

                                                        // Apply window to data
        SDA_Window (pRealData,                          // Pointer to source array
                    pRealData,                          // Pointer to destination array
                    pWindowCoeffs,                      // Pointer to window oefficients
                    WINDOW_SIZE);                       // Window length

                                                        // Perform real FFT
        SDA_Rfft (pRealData,                            // Pointer to real array
                  pImagData,                            // Pointer to imaginary array
                  pFFTCoeffs,                           // Pointer to FFT coefficients
                  SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
                  FFT_LENGTH,                           // FFT length
                  LOG2_FFT_LENGTH);                     // log2 FFT length

                                                        // Calculate magnitude
        SDA_Magnitude (pRealData,                       // Pointer to real source array
                       pImagData,                       // Pointer to imaginary source array
                       pResults,                        // Pointer to log magnitude destination array
                       FFT_HALF_LENGTH);                // Dataset length

        SDA_Multiply (pResults,                         // Pointer to real source array
                      windowInverseCoherentGain / FFT_HALF_LENGTH,  // Pointer to imaginary source array
                      pResults,                         // Pointer to log magnitude destination array
                      FFT_HALF_LENGTH);                 // Dataset length

                                                    // Interpolate the magnitude and frequency
        SLData_t interpolatedMagn =
            SDA_InterpolateArbitraryThreePointQuadraticPeakVertexMagnitude (pResults,           // Pointer to source array
                                                                            FFT_HALF_LENGTH);   // Dataset length
        SLData_t interpolatedFreq =
            SDA_InterpolateArbitraryThreePointQuadraticPeakVertexLocation (pResults,            // Pointer to source array
                                                                           FFT_HALF_LENGTH);    // Dataset length
        interpolatedFreq /= FFT_LENGTH;

        magnErrorPercent[i] = SIGLIB_ONE_HUNDRED * ((interpolatedMagn - SOURCE_MAGN) / SOURCE_MAGN);    // Save results to plot later
        freqErrorPercent[i] = SIGLIB_ONE_HUNDRED * ((interpolatedFreq - centerFreq) / centerFreq);
    }

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 magnErrorPercent,                  // Dataset
                 NUM_FREQS,                         // Dataset length
                 "Magnitude Error Percentage",      // Dataset title
                 FREQ_START,                        // Minimum X value
                 FREQ_END,                          // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 freqErrorPercent,                  // Dataset
                 NUM_FREQS,                         // Dataset length
                 "Frequency Error Percentage",      // Dataset title
                 FREQ_START,                        // Minimum X value
                 FREQ_END,                          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_ADD);                          // New graph

    printf ("Max gain error percentage = %lf, max frequency error percentage = %lf\n", SDA_AbsMax (magnErrorPercent, NUM_FREQS), SDA_AbsMax (freqErrorPercent, NUM_FREQS));
    printf ("Magnitude / Frequency Percentage Error\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    SUF_MemoryFree (pRealData);                     // Free memory
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pWindowCoeffs);
    SUF_MemoryFree (pFFTCoeffs);
    SUF_MemoryFree (pResults);
}


