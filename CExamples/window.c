// SigLib Time Domain Windowing Example
// This program also shows how to use the window normalised gain compensation function
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define FFT_LENGTH      512
#define LOG2_FFT_LENGTH ((SLArrayIndex_t)(SDS_Log2(FFT_LENGTH)+SIGLIB_MIN_THRESHOLD))   // Log FFT length and avoid quantization issues

//#define HALF_FFT_LENGTH FFT_LENGTH / 2

#define WINDOW_LENGTH   63

static SLData_t     *pWindowCoeffs, *pFFTCoeffs, *pRealData, *pImagData, *pResults;

// Declare global variables and arrays
static SLData_t     *pSrc, *pDst, *pWindowCoeffs, *pFFTCoeffs, *pRealData, *pImagData, *pResults;

void prepFFT (void);

void main(void)

{
    h_GPC_Plot  *h2DTime;                           // Plot object
    h_GPC_Plot  *h2DFreq;

    h2DTime =                                       // Initialize plot
        gpc_init_2d ("Time Domain Windowing",       // Plot title
                     "Time",                        // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (NULL == h2DTime) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    h2DFreq =                                       // Initialize plot
        gpc_init_2d ("Time Domain Windowing",       // Plot title
                     "Frequency",                   // X-Axis label
                     "Magnitude (dB)",              // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (NULL == h2DTime) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

                                                    // Allocate memory
    pSrc = SUF_VectorArrayAllocate (WINDOW_LENGTH);
    pDst = SUF_VectorArrayAllocate (WINDOW_LENGTH);
    pWindowCoeffs = SUF_VectorArrayAllocate (WINDOW_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);
    pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);

                                                    // Generate Rectangular window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_RECTANGLE,                   // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length


    printf ("\nRectangle window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Rectangle Window",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "violet",                          // Colour
                 GPC_NEW);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Rectangle Window",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "violet",                          // Colour
                 GPC_NEW);                          // New graph

                                                    // Generate Hanning window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_HANNING,                     // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length


    printf ("\nHanning window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Hanning Window",                  // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Hanning Window",                  // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate table top Hanning window table
    SIF_TableTopWindow (pWindowCoeffs,              // Pointer to window oefficient
                        SIGLIB_HANNING,             // Window type
                        SIGLIB_ZERO,                // Window coefficient
                        12,                         // Flat top section length
                        WINDOW_LENGTH);             // Window length

    printf ("Table-Top Hanning window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Table-Top Hanning Window",        // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Table-Top Hanning Window",        // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_HAMMING,                     // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    printf ("Hamming window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Hamming Window",                  // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "yellow",                          // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Hamming Window",                  // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "yellow",                          // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_BLACKMAN,                    // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    printf ("Blackman window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Blackman Window",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "green",                           // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Blackman Window",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "green",                           // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_BARTLETT_TRIANGLE_ZERO_END_POINTS,   // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    printf ("Bartlett / Triangle window (zero end points) inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Bartlett / Triangle Window (zero end points)", // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "orange",                          // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Bartlett / Triangle Window (zero end points)", // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "orange",                          // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_BARTLETT_TRIANGLE_NON_ZERO_END_POINTS,   // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    printf ("Bartlett / Triangle window (non-zero end points) inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Bartlett / Triangle Window (non-zero end points)", // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "orange-red",                      // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Bartlett / Triangle Window (non-zero end points)", // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "orange-red",                      // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_KAISER,                      // Window type
                SIGLIB_SIX,                         // Window coefficient
                WINDOW_LENGTH);                     // Window length

    printf ("Kaiser window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Kaiser Window",                   // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "grey",                            // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Kaiser Window",                   // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "grey",                            // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_BMAN_HARRIS,                 // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    printf ("Blackman-Harris window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Blackman-Harris Window",          // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "cyan",                            // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Blackman-Harris Window",          // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "cyan",                            // Colour
                 GPC_ADD);                          // New graph

                                                    // Generate flat-top window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_FLAT_TOP,                    // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length

    printf ("Flat-Top window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));
    gpc_plot_2d (h2DTime,                           // Graph handle
                 pWindowCoeffs,                     // Dataset
                 WINDOW_LENGTH,                     // Dataset length
                 "Flat-Top Window",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(WINDOW_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "purple",                          // Colour
                 GPC_ADD);                          // New graph
    prepFFT();
    gpc_plot_2d (h2DFreq,                           // Graph handle
                 pResults,                          // Dataset
                 FFT_LENGTH,                        // Dataset length
                 "Flat-Top Window",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(FFT_LENGTH - 1),          // Maximum X value
                 "lines",                           // Graph type
                 "purple",                          // Colour
                 GPC_ADD);                          // New graph

                 printf ("Please hit <Carriage Return> to continue . . ."); getchar ();


                                                    // Generate window table
    SIF_Window (pWindowCoeffs,                      // Pointer to window oefficient
                SIGLIB_BMAN_HARRIS,                 // Window type
                SIGLIB_ZERO,                        // Window coefficient
                WINDOW_LENGTH);                     // Window length
                                                    // Apply window to real data
    SDA_Window (pSrc,                               // Pointer to source array
                pDst,                               // Pointer to destination array
                pWindowCoeffs,                      // Pointer to window coefficients
                WINDOW_LENGTH);                     // Window length

    printf ("\n\nBlackman-Harris Window\n");
    printf ("RMS Sum of sine wave = %lf\n", SDA_RootMeanSquare (pSrc, WINDOW_LENGTH));
    printf ("RMS Sum of window = %lf\n", SDA_RootMeanSquare (pWindowCoeffs, WINDOW_LENGTH));
    printf ("RMS Sum of windowed data = %lf\n", SDA_RootMeanSquare (pDst, WINDOW_LENGTH));
    printf ("Window inverse coherent gain = %lf\n", SDA_WindowInverseCoherentGain (pWindowCoeffs, WINDOW_LENGTH));

    gpc_close (h2DTime);
    gpc_close (h2DFreq);

    SUF_MemoryFree (pSrc);                          // Free memory
    SUF_MemoryFree (pDst);
    SUF_MemoryFree (pWindowCoeffs);
}


void prepFFT (void)                                 // Macro to generate the FFT output
{
    SDA_Lengthen (pWindowCoeffs, pRealData, WINDOW_LENGTH, FFT_LENGTH);
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
                      FFT_LENGTH);                  // Dataset length
    SDA_LogMagnitude (pRealData, pImagData, pRealData, FFT_LENGTH);     // Calc power in dB
    SDA_FftShift (pRealData, pResults, FFT_LENGTH);
    SDA_NegativeOffset (pResults,                   // Pointer to source array
                        pResults,                   // Pointer to destination array
                        FFT_LENGTH);                // Dataset length
    SDA_Clip (pResults,                             // Pointer to results array
              pResults,                             // Pointer to results array
              SIGLIB_DB_MIN,                        // Clip level
              SIGLIB_CLIP_BELOW,                    // Clip direction
              FFT_LENGTH);                          // Dataset length
}

