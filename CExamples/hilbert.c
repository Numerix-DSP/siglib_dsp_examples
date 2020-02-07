// SigLib Hilbert Transform Example

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH               512
#define HILBERT_TRANSFORM_LENGTH    41
#define DELAY_LENGTH                (HILBERT_TRANSFORM_LENGTH >> 1)

// Declare global variables and arrays
static SLData_t         *pSrc;
static SLData_t         *pFilterCoeffs, *pFilterState;
static SLData_t         *pDelayState;
static SLData_t         *pHilbertTransformed, *pDelayed;
static SLArrayIndex_t   FilterIndex, DelayIndex;
static SLData_t         SinePhase;

void    main(void);

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    SLArrayIndex_t  i;

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Hilbert Transform",           // Plot title
                     "Time",                        // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    pSrc = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pFilterCoeffs = SUF_VectorArrayAllocate (HILBERT_TRANSFORM_LENGTH);
    pFilterState = SUF_VectorArrayAllocate (HILBERT_TRANSFORM_LENGTH);
    pHilbertTransformed = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pDelayState = SUF_VectorArrayAllocate (DELAY_LENGTH);
    pDelayed = SUF_VectorArrayAllocate (SAMPLE_LENGTH);

                                                    // Initialise Hilbert transformer coefficients
    SIF_HilbertTransformer (pFilterCoeffs,          // Pointer to filter coefficients
                            HILBERT_TRANSFORM_LENGTH);  // Filter length
                                                    // Initialise FIR filter for Hilbert transformer
    SIF_Fir (pFilterState,                          // Pointer to filter state array
             &FilterIndex,                          // Pointer to filter index register
             HILBERT_TRANSFORM_LENGTH);             // Filter length
                                                    // Initialise the delay
    SIF_FixedDelay (pDelayState,                    // Pointer to delay state array
                    &DelayIndex,                    // Pointer to delay index
                    DELAY_LENGTH);                  // Delay length

    SinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pSrc,                       // Pointer to destination array
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.01,                       // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc,                              // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Source Signal",                   // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSource Signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Apply Hilbert transformerer
    SDA_Fir (pSrc,                                  // Input array to be filtered
             pHilbertTransformed,                   // Filtered output array
             pFilterState,                          // Pointer to filter state array
             pFilterCoeffs,                         // Pointer to filter coefficients
             &FilterIndex,                          // Pointer to filter index register
             HILBERT_TRANSFORM_LENGTH,              // Filter length
             SAMPLE_LENGTH);                        // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pHilbertTransformed,               // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Hilbert Transformed Signal",      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nHilbert Transformed Signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                        // Delay input signal
//  SDA_FixedDelay (pSrc, pDelayed, pDelayState, &DelayIndex, DELAY_LENGTH, SAMPLE_LENGTH);

    for (i = 0; i < SAMPLE_LENGTH; i++) {
        *(pDelayed+i) =
            SDS_FixedDelay (*(pSrc+i),              // Input data sample
                            pDelayState,            // Pointer to delay state array
                            &DelayIndex,            // Pointer to delay state index
                            DELAY_LENGTH);          // Delay length
    }

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pDelayed,                          // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Delayed Signal",                  // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "magenta",                         // Colour
                 GPC_NEW);                          // New graph
    printf ("\nDelayed Signal\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pSrc);                          // Free memory
    SUF_MemoryFree (pFilterCoeffs);
    SUF_MemoryFree (pFilterState);
    SUF_MemoryFree (pHilbertTransformed);
    SUF_MemoryFree (pDelayState);
    SUF_MemoryFree (pDelayed);
}


