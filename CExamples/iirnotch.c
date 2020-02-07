// IIR Notch Filter Example
// Generates the notch filter with the specified magnitude for the pole

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define NOTCH_FREQUENCY         0.125               // Frequency of the notch
#define POLE_MAGNITUDE          0.9                 // Magnitude of the pole

#define SAMPLE_RATE             SIGLIB_ONE          // Normalized to 1.0 Hz for convenience

#define FILTER_ORDER            2                   // Filter order
#define FILTER_STAGES           ((FILTER_ORDER+1) >> 1)     // Number of 2nd-order filter stages

#define IMPULSE_RESPONSE_LENGTH 1024

#define FFT_LENGTH              IMPULSE_RESPONSE_LENGTH
#define LOG2_FFT_LENGTH         10

#define PLOT_LENGTH             (IMPULSE_RESPONSE_LENGTH/2)

// Declare global variables and arrays
static SLData_t     *pFilterState, *pIIRCoeffs;
static SLData_t     *pSrc, *pRealData, *pImagData, *pResults, *pFFTCoeffs;


void main (void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

                                                    // Allocate memory
    pIIRCoeffs = SUF_IirCoefficientAllocate (FILTER_STAGES);
    pFilterState = SUF_IirStateArrayAllocate (FILTER_STAGES);
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (PLOT_LENGTH);
    pSrc = SUF_VectorArrayAllocate (IMPULSE_RESPONSE_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

    if ((pIIRCoeffs == NULL) || (pRealData == NULL) || (pImagData == NULL) || (pResults == NULL) ||
        (pSrc == NULL) || (pFFTCoeffs == NULL)) {

        printf ("\n\nMemory allocation failed\n\n");
        exit (0);
    }

                                                    // Generate the notch filter coefficients
    SIF_IirNotchFilter2 (pIIRCoeffs,                 // Pointer to filter coefficients
                        NOTCH_FREQUENCY,            // Notch frequency
                        POLE_MAGNITUDE,             // Pole magnitude
                        FILTER_ORDER);              // Filter order

                                                    // Generate impulse response
    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("IIR Notch Filter",            // Plot title
                     "Time / Frequency",            // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    SIF_Iir (pFilterState,                          // Pointer to filter state array
             FILTER_STAGES);                        // Number of second order stages
                                                    // Initialise FFT
    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length

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

                                                    // Apply iir filter and store filtered data
    SDA_Iir (pSrc,                                  // Input array to be filtered
             pRealData,                             // Filtered output array
             pFilterState,                          // Pointer to filter state array
             pIIRCoeffs,                            // Pointer to filter coefficients array
             FILTER_STAGES,                         // Number of stages
             IMPULSE_RESPONSE_LENGTH);              // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 PLOT_LENGTH,                       // Dataset length
                 "Impulse response",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(PLOT_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nImpulse response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Generate frequency response
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
                 "Frequency response",              // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(PLOT_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nFrequency response\n");


    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pSrc);                          // Free memory
    SUF_MemoryFree (pIIRCoeffs);
    SUF_MemoryFree (pFilterState);
    SUF_MemoryFree (pRealData);
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pResults);
    SUF_MemoryFree (pFFTCoeffs);
}
