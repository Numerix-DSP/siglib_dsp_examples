// SigLib IIR 2 Pole Low Pass Filter Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define PLOT_RESULTS            1                   // Set to 1 to plot results as well as print them

#define SAMPLE_RATE             10000.              // 10 KHz sample rate
#define CUT_OFF_FREQUENCY       1000.               // Cut-off frequency
#define RADIUS                  0.9                 // Radius

#define SAMPLE_LENGTH           512
#define IMPULSE_RESPONSE_LENGTH 1024
#define PLOT_LENGTH             (IMPULSE_RESPONSE_LENGTH/2)
#define FFT_LENGTH              1024
#define LOG2_FFT_LENGTH         ((SLArrayIndex_t)(SDS_Log2(FFT_LENGTH)+SIGLIB_MIN_THRESHOLD))   // Log FFT length and avoid quantization issues

#define FILTER_ORDER            2                           // Filter length
#define IIR_FILTER_STAGES       ((FILTER_ORDER+1)>>1)       // Number of biquads in filter
#define FILTER_COEFF_ARRAY_LENGTH   (IIR_FILTER_STAGES * SIGLIB_IIR_COEFFS_PER_BIQUAD)  // Number of filter coefficients

// Declare global variables and arrays
#if PLOT_RESULTS
static SLData_t     *pFilterState, *pIIRCoeffs;
static SLData_t     *pSrc, *pRealData, *pImagData, *pResults, *pFFTCoeffs;
#endif

void main (void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

#if (PLOT_RESULTS == 0)
    SLArrayIndex_t  i;
#endif

#if PLOT_RESULTS
                                                        // Allocate memory
    pIIRCoeffs = SUF_IirCoefficientAllocate (IIR_FILTER_STAGES);
    pFilterState = SUF_IirStateArrayAllocate (IIR_FILTER_STAGES);
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (PLOT_LENGTH);
    pSrc = SUF_VectorArrayAllocate (IMPULSE_RESPONSE_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

    if ((NULL == pIIRCoeffs) || (NULL == pRealData) || (NULL == pImagData) || (NULL == pResults) ||
        (NULL == pSrc) || (NULL == pFFTCoeffs)) {

        printf ("\n\nMemory allocation failed\n\n");
        exit (0);
    }

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("IIR 2 Pole Low-Pass Filter Example",    // Plot title
                     "Time / Frequency",            // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (NULL == h2DPlot) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

                                                    // Initialise FFT
    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length
#endif

                                                    // Original coefficients
                                                    // Generate test impulse
    SDA_SignalGenerate (pSrc,                       // Pointer to destination array
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        IMPULSE_RESPONSE_LENGTH);   // Output dataset length

                                                    // Initialize the IIR filter function
    SIF_Iir2PoleLpf (pFilterState,                  // Pointer to filter state array
                     pIIRCoeffs,                    // Pointer to filter coefficients array
                     CUT_OFF_FREQUENCY / SAMPLE_RATE,   // Cut-off frequency
                     RADIUS);                       // Pole radius

    printf ("Coeff [0] = %lf, Coeff [1] = %lf\n", *pIIRCoeffs, *(pIIRCoeffs+1));

                                                    // Apply iir filter and store filtered data
    SDA_Iir2Pole (pSrc,                             // Input array to be filtered
                  pRealData,                        // Filtered output array
                  pFilterState,                     // Pointer to filter state array
                  pIIRCoeffs,                       // Pointer to filter coefficients array
                  IMPULSE_RESPONSE_LENGTH);         // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 PLOT_LENGTH,                       // Dataset length
                 "Source - Impulse Response",       // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSource - Impulse Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Generate frequency response
                                                    // Perform real FFT
    SDA_Rfft (pRealData,                            // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real power from complex
    SDA_LogMagnitude (pRealData,                    // Pointer to real source array
                      pImagData,                    // Pointer to imaginary source array
                      pResults,                     // Pointer to log magnitude destination array
                      PLOT_LENGTH);                 // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 PLOT_LENGTH,                       // Dataset length
                 "Original Frequency Response",     // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nFrequency Response\nPlease hit <Carriage Return> to continue . . .");

#if PLOT_RESULTS
    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pSrc);                          // Free memory
    SUF_MemoryFree (pIIRCoeffs);
    SUF_MemoryFree (pFilterState);
    SUF_MemoryFree (pRealData);
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pResults);
    SUF_MemoryFree (pFFTCoeffs);
#endif

}


