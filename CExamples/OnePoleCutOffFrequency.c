// SigLib One-pole Filter Example, with cut-off frequency
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib_host_utils.h>                      // Optionally includes conio.h and time.h subset functions
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH       512
#define FFT_LENGTH          512
#define HALF_FFT_LENGTH     (FFT_LENGTH >> 1)
#define LOG2_FFT_LENGTH     ((SLArrayIndex_t)(SDS_Log2(FFT_LENGTH)+SIGLIB_MIN_THRESHOLD))   // Log FFT length and avoid quantization issues

// Declare global variables and arrays
static SLData_t     *pRealData, *pImagData, *pResults, *pSrc1, *pSrc2, *pFFTCoeffs;
static SLData_t     Fc, OnePoleCoeff, OnePoleHighPassCoeff, Offset, OnePoleFilterState, OnePoleHighPassFilterState, SinePhase;





void main(int argc,char **argv)
{
    h_GPC_Plot  *h2DPlot;                                   // Plot object

    if (argc != 2) {
        printf ("Usage   : OnePole <Fc>\n");
        printf ("Example : OnePole 0.2\n");
        printf ("(Nominal sample rate = 1 Hz)\n\n");
        Fc = 0.2;
    }
    else {
        Fc = (SLData_t) atof (argv[1]);
    }

    OnePoleCoeff =
        SDS_OnePoleCutOffFrequencyToFilterCoeff (Fc,                    // Cut-off frequency (Hz)
                                                 SIGLIB_ONE);           // Sample rate
    printf ("One-pole filter coefficient: %lf\n", OnePoleCoeff);

    OnePoleHighPassCoeff =
        SDS_OnePoleHighPassCutOffFrequencyToFilterCoeff (Fc,            // Cut-off frequency (Hz)
                                                         SIGLIB_ONE);   // Sample rate
    printf ("One-pole high pass filter coefficient: %lf\n", OnePoleHighPassCoeff);

                                                            // Allocate memory
    pSrc1 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pSrc2 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);        // RMS result array
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

                                                            // Initialise FFT
    SIF_Fft (pFFTCoeffs,                                    // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,                   // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                                   // FFT length

    SDA_SignalGenerate (pSrc1,                              // Pointer to destination array
                        SIGLIB_IMPULSE,                     // Signal type - impulse function
                        SIGLIB_ONE,                         // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                        // Signal frequency - Unused
                        SIGLIB_ZERO,                        // D.C. Offset
                        SIGLIB_ZERO,                        // Delay (samples periods) applied to step
                        SIGLIB_ZERO,                        // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,               // Unused
                        SIGLIB_NULL_DATA_PTR,               // Unused
                        SAMPLE_LENGTH);                     // Output dataset length

   h2DPlot =                                                // Initialize plot
       gpc_init_2d ("One-pole Filter",                      // Plot title
                    "Time (s)",                             // X-Axis label
                    "Magnitude",                            // Y-Axis label
                    3.,                                     // Scaling mode
                    GPC_SIGNED,                             // Sign mode
                    GPC_KEY_ENABLE);                        // Legend / key mode
   if (NULL == h2DPlot) {
       printf ("\nPlot creation failure.\n");
       exit (1);
   }

    SIF_OnePole (&OnePoleFilterState);                      // Filter state

    SDA_OnePoleNormalized (pSrc1,                           // Source data array pointer
                           pRealData,                       // Destination data array pointer
                           OnePoleCoeff,                    // Filter coefficient
                           &OnePoleFilterState,             // Filter state
                           SAMPLE_LENGTH);                  // Dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pRealData,                                 // Dataset
                 20,                                        // Dataset length
                 "Impulse Response Of One-pole Filter",     // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 SIGLIB_HALF,                               // Maximum X value
                 "lines",                                   // Graph type
                 "blue",                                    // Colour
                 GPC_NEW);                                  // New graph

    SIF_OnePoleHighPass (&OnePoleHighPassFilterState);      // Filter state

    SDA_OnePoleHighPassNormalized (pSrc1,                       // Source data array pointer
                                   pRealData,                   // Destination data array pointer
                                   OnePoleHighPassCoeff,        // Filter coefficient
                                   &OnePoleHighPassFilterState, // Filter state
                                   SAMPLE_LENGTH);              // Dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pRealData,                                 // Dataset
                 20,                                        // Dataset length
                 "Impulse Response Of One-pole High Pass Filter",     // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 SIGLIB_HALF,                               // Maximum X value
                 "lines",                                   // Graph type
                 "red",                                     // Colour
                 GPC_ADD);                                  // New graph
    printf ("\nImpulse Response Of One-pole Filters\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_close (h2DPlot);


    h2DPlot =                                               // Initialize plot
        gpc_init_2d ("One-pole Filter Frequency Response",  // Plot title
                     "Frequency",                           // X-Axis label
                     "Magnitude",                           // Y-Axis label
                     GPC_AUTO_SCALE,                        // Scaling mode
                     GPC_SIGNED,                            // Sign mode
                     GPC_KEY_DISABLE);                      // Legend / key mode
    if (NULL == h2DPlot) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    SIF_OnePole (&OnePoleFilterState);                      // Filter state

    SDA_OnePoleNormalized (pSrc1,                           // Source data array pointer
                           pRealData,                       // Destination data array pointer
                           OnePoleCoeff,                    // Filter coefficient
                           &OnePoleFilterState,             // Filter state
                           SAMPLE_LENGTH);                  // Dataset length

                                                            // Perform real FFT
    SDA_Rfft (pRealData,                                    // Pointer to real array
              pImagData,                                    // Pointer to imaginary array
              pFFTCoeffs,                                   // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,                  // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                                   // FFT length
              LOG2_FFT_LENGTH);                             // log2 FFT length

                                                            // Calculate real power from complex
    SDA_LogMagnitude (pRealData,                            // Pointer to real source array
                      pImagData,                            // Pointer to imaginary source array
                      pResults,                             // Pointer to log magnitude destination array
                      HALF_FFT_LENGTH);                     // Dataset length

    SDA_Clip (pResults,                                     // Pointer to results array
              pResults,                                     // Pointer to results array
              -10.,                                         // Clip level
              SIGLIB_CLIP_BELOW,                            // Clip direction
              HALF_FFT_LENGTH);                             // Dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pResults,                                  // Dataset
                 HALF_FFT_LENGTH,                           // Dataset length
                 "Frequency Response Of One-pole Filter",   // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 SIGLIB_HALF,                               // Maximum X value
                 "lines",                                   // Graph type
                 "blue",                                    // Colour
                 GPC_NEW);                                  // New graph


    SIF_OnePoleHighPass (&OnePoleHighPassFilterState);      // Filter state

    SDA_OnePoleHighPassNormalized (pSrc1,                       // Source data array pointer
                                   pRealData,                   // Destination data array pointer
                                   OnePoleHighPassCoeff,        // Filter coefficient
                                   &OnePoleHighPassFilterState, // Filter state
                                   SAMPLE_LENGTH);              // Dataset length

                                                            // Perform real FFT
    SDA_Rfft (pRealData,                                    // Pointer to real array
              pImagData,                                    // Pointer to imaginary array
              pFFTCoeffs,                                   // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,                  // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                                   // FFT length
              LOG2_FFT_LENGTH);                             // log2 FFT length

                                                            // Calculate real power from complex
    SDA_LogMagnitude (pRealData,                            // Pointer to real source array
                      pImagData,                            // Pointer to imaginary source array
                      pResults,                             // Pointer to log magnitude destination array
                      HALF_FFT_LENGTH);                     // Dataset length

    SDA_Clip (pResults,                                     // Pointer to results array
              pResults,                                     // Pointer to results array
              -10.,                                         // Clip level
              SIGLIB_CLIP_BELOW,                            // Clip direction
              HALF_FFT_LENGTH);                             // Dataset length

    gpc_plot_2d (h2DPlot,                                   // Graph handle
                 pResults,                                  // Dataset
                 HALF_FFT_LENGTH,                           // Dataset length
                 "Frequency Response Of One-pole High Pass Filter",   // Dataset title
                 SIGLIB_ZERO,                               // Minimum X value
                 SIGLIB_HALF,                               // Maximum X value
                 "lines",                                   // Graph type
                 "red",                                     // Colour
                 GPC_ADD);                                  // New graph
    printf ("\nFrequency Response Of One-pole Filters\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_close (h2DPlot);

    SUF_MemoryFree (pSrc1);                                 // Free memory
    SUF_MemoryFree (pSrc2);
    SUF_MemoryFree (pRealData);
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pResults);
    SUF_MemoryFree (pFFTCoeffs);
}

