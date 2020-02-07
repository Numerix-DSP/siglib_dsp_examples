// SigLib simple envelope detector using Hilbert Transform
// This example works for simple sinusoidal signals however a filter
// based solution maybe required for real-time applications.
// See example envelope-hilbert.c


// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH   512
#define FFT_LENGTH      SAMPLE_LENGTH
#define LOG2_FFT_LENGTH 9

#define FILTER_LENGTH   31
#define FILTER_GROUP_DELAY  (FILTER_LENGTH >> 1)    // Filter group delay - Note : C array indexing

// Declare global variables and arrays
static SLData_t         SinePhase;
static SLData_t         *pMagnitude, *pDelay, *pTempDelay;
static SLData_t         *pFilterTaps, *pFilterTapsStart, *pFilterState;
static SLArrayIndex_t   FilterIndex;
static SLData_t         *pSrc, *pHilbertTransformed;

void    main(void);


void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Analytic Signal",             // Plot title
                     "Time",                        // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_DISABLE);              // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    printf ("Hilbert transform filter length   => %d", FILTER_LENGTH);

    pFilterTaps = SUF_VectorArrayAllocate (FILTER_LENGTH);
    pFilterState = SUF_VectorArrayAllocate (FILTER_LENGTH);
    pSrc = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pHilbertTransformed = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pMagnitude = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pDelay = SUF_VectorArrayAllocate (FILTER_GROUP_DELAY);
    pTempDelay = SUF_VectorArrayAllocate (FILTER_GROUP_DELAY);

    pFilterTapsStart = pFilterTaps;
                                                    // Initialise Hilbert transformer coefficients
    SIF_HilbertTransformer (pFilterTaps,            // Pointer to filter coefficients
                            FILTER_LENGTH);         // Filter length
                                                    // Initialise FIR filter for Hilbert transformer
    SIF_Fir (pFilterState,                          // Pointer to filter state array
             &FilterIndex,                          // Pointer to filter index register
             FILTER_LENGTH);                        // Filter length
                                                    // Initialise the delay function
    SIF_FixedDelay (pDelay,                         // Pointer to filter index register
                    SIGLIB_NULL_ARRAY_INDEX_PTR,    // Unused
                    FILTER_GROUP_DELAY);            // Filter length

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pSrc,                       // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.05,                       // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pHilbertTransformed,        // Pointer to destination array - reuse pHilbertTransformed array for temp data
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    SDA_Multiply2 (pSrc, pHilbertTransformed, pSrc,  SAMPLE_LENGTH);    // Multiply two signals to create input

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc,                              // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Source signal",                   // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSource signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    SDA_Copy (pSrc,                                 // Pointer to source array
              pHilbertTransformed,                  // Pointer to destination array
              SAMPLE_LENGTH);                       // Dataset length

                                                    // Apply Hilbert transformerer
    SDA_Fir (pSrc,                                  // Input array to be filtered
             pHilbertTransformed,                   // Filtered output array
             pFilterState,                          // Pointer to filter state array
             pFilterTaps,                           // Pointer to filter coefficients
             &FilterIndex,                          // Pointer to filter index register
             FILTER_LENGTH,                         // Filter length
             SAMPLE_LENGTH);                        // Dataset length

                                                    // Delay real component to compensate for filter delay
    SDA_ShortFixedDelay (pSrc,                      // Pointer to source array
                         pSrc,                      // Pointer to destination array
                         pDelay,                    // Pointer to temporary delayed array
                         pTempDelay,                // Temporary destination array pointer
                         FILTER_GROUP_DELAY,        // Sample delay count
                         SAMPLE_LENGTH);            // Dataset length

                                                    // Calculate real magnitude from complex
    SDA_Magnitude (pSrc,                            // Pointer to real source array
                   pHilbertTransformed,             // Pointer to imaginary source array
                   pMagnitude,                      // Pointer to magnitude destination array
                   SAMPLE_LENGTH);                  // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pMagnitude,                        // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Envelope",                        // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nEnvelope\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pFilterTaps);                   // Free memory
    SUF_MemoryFree (pFilterState);
    SUF_MemoryFree (pSrc);
    SUF_MemoryFree (pHilbertTransformed);
    SUF_MemoryFree (pMagnitude);
    SUF_MemoryFree (pDelay);
    SUF_MemoryFree (pTempDelay);

}

