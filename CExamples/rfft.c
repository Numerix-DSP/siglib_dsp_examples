// SigLib Real FFT Example
// This program shows that the magnitude results of the real FFT operation are (N/2)+1 long.
// The reason for this is that the results in bins 0 and N/2 are purely real.
// The results are plotted on the screen and also written to the debug.log file. The FFT is
// 16 samples long and the data in the log file is stored on two lines of 8 for each result.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define FFT_LENGTH      16
#define LOG2_FFT_LENGTH 4

// Declare global variables and arrays
static SLData_t     *pRealData, *pImagData, *pResults, *pFFTCoeffs;
static SLData_t     RampPhase;

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);   // Allocate memory
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Real FFT",                    // Plot title
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
             FFT_LENGTH);                               // FFT length

                                                    // Initialise the source data to a ramp
    RampPhase = SIGLIB_ZERO;
    SDA_SignalGenerateRamp  (pRealData,                             // Pointer to destination array
                             ((SLData_t)FFT_LENGTH)/SIGLIB_TWENTY,  // Amplitude
                             ((SLData_t)FFT_LENGTH)/SIGLIB_TWENTY,  // D.C. Offset
                             &RampPhase,                            // Phase - maintained across array boundaries
                             FFT_LENGTH);                           // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Source Signal",                   // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSource Signal (Ramp)\nPlease hit <Carriage Return> to continue . . ."); getchar ();

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
                 pRealData,                         // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Real Result",                     // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_NEW);                          // New graph

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pImagData,                         // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Imaginary Result",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "yellow",                          // Colour
                 GPC_ADD);                          // New graph

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Magnitude Signal",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_ADD);                          // New graph
    printf ("\nReal, Imaginary and Magnitude Results\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pRealData);                     // Free memory
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pResults);
    SUF_MemoryFree (pFFTCoeffs);
}

