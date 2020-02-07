// SigLib - Signal Generator Example

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH           512
#define FFT_LENGTH              512
#define LOG2_FFT_LENGTH         9

#define GAUS_NOISE_VARIANCE     SIGLIB_FOUR
#define GAUS_NOISE_OFFSET       SIGLIB_ZERO

#define OUTPUT_MAGNITUDE        SIGLIB_ONE                  // Magnitude of output signals

#define SAMPLE_RATE             44100.                      // Characteristics of linear chirp signal
#define CHIRP_MAGNITUDE         SIGLIB_ONE
#define CHIRP_START_FREQ        1800.0
#define CHIRP_END_FREQ          18000.0

#define HGRAM_SAMPLE_LENGTH     64

// Declare global variables and arrays
static SLData_t     *pRealData, *pImagData, *pResults, *pFFTCoeffs;     // Dataset pointers

static SLData_t     SinePhase, CosinePhase, SqPhase, TriPhase, ImpulsePhase;
static SLData_t     ChirpPhase, ChirpValue;
static SLData_t     GaussPhase, GaussValue;
static SLData_t     PnsPhase, PnsCurrentValue;

void main (void)

{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    SLData_t    RMSValue;
    char        OutputString [80];

    pRealData = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
    pResults = SUF_VectorArrayAllocate (FFT_LENGTH);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

    printf ("\nSignal generation examples - simulated sample rate = %lf\n", SAMPLE_RATE);

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Signal Generation Example",   // Plot title
                     "Time",                        // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Impulse waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        51.,                        // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Delayed impulse waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_WHITE_NOISE,         // Signal type - random white noise
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "White noise waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_Clip (pRealData, pRealData, SIGLIB_HALF, SIGLIB_CLIP_BOTH, SAMPLE_LENGTH);
    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Clipped white noise waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SinePhase = SIGLIB_ZERO;
    CosinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Sine waveform 1, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Sine waveform 2, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_COS_WAVE,            // Signal type - Cosine wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.01,                       // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &CosinePhase,               // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Cosine waveform 1, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SinePhase = SIGLIB_ZERO;
    CosinePhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_SINE_WAVE,           // Signal type - Sine wave
                        SIGLIB_HALF,                // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        0.2,                        // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SinePhase,                 // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Sine waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_COS_WAVE,            // Signal type - Cosine wave
                        SIGLIB_HALF,                // Signal peak level
                        SIGLIB_ADD,                 // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        -0.2,                       // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &CosinePhase,               // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Sin + Cos waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SqPhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_SQUARE_WAVE,         // Signal type - Square wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        0.75,                       // Duty cycle
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SqPhase,                   // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Square waveform 1, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_SQUARE_WAVE,         // Signal type - Square wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_QUARTER,             // Duty cycle
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &SqPhase,                   // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Square waveform 2, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    TriPhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_TRIANGLE_WAVE,       // Signal type - Triangular wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Equilateral waveform
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &TriPhase,                  // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Triangle waveform 1, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_TRIANGLE_WAVE,       // Signal type - Triangular wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Equilateral waveform
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &TriPhase,                  // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Triangle waveform 1, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_TRIANGLE_WAVE,       // Signal type - Triangular wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Equilateral waveform
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &TriPhase,                  // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    TriPhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_TRIANGLE_WAVE,       // Signal type - Triangular wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_MINUS_ONE,           // Ramp down waveform
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &TriPhase,                  // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Triangle waveform 2, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_TRIANGLE_WAVE,       // Signal type - Triangular wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_MINUS_ONE,           // Ramp down waveform
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &TriPhase,                  // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Triangle waveform 2, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    TriPhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_TRIANGLE_WAVE,       // Signal type - Triangular wave
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.005,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ONE,                 // Ramp up waveform
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &TriPhase,                  // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Triangle waveform 3, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_IMPULSE_STREAM,      // Signal type - Impulse stream
                        OUTPUT_MAGNITUDE,           // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.051,                      // Signal frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Unused
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &ImpulsePhase,              // Signal phase - maintained across array boundaries
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);


    sprintf (OutputString, "Impulse stream waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


    PnsPhase = SIGLIB_ZERO;
                                                    // Create a uni-polar PRN sequence
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_PN_SEQUENCE,         // Signal type - Pseudo random number sequence
                        OUTPUT_MAGNITUDE,           // Signal magnitude range
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.05,                       // Signal frequency
                        SIGLIB_ZERO,                // Signal minimum level
                        SIGLIB_FOUR,                // Number of discrete levels in PN sequence
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &PnsPhase,                  // PRN phase - used for next iteration
                        &PnsCurrentValue,           // PRN current value - used for next iteration
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "PN sequence 1 waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


                                                    // Create a bi-polar PRN sequence
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_PN_SEQUENCE,         // Signal type - Pseudo random number sequence
                        SIGLIB_TWO,                 // Signal magnitude range
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.05,                       // Signal frequency
                        SIGLIB_MINUS_ONE,           // Signal minimum level
                        SIGLIB_EIGHT,               // Number of discrete levels in PN sequence
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &PnsPhase,                  // PRN phase - used for next iteration
                        &PnsCurrentValue,           // PRN current value - used for next iteration
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "PN sequence 2 waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


                                                    // Initialise FFT
    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length

    ChirpPhase = SIGLIB_ZERO;
    ChirpValue = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_CHIRP_NL,            // Signal type - Chirp with non linear frequency ramp
                        0.45,                       // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        0.0025,                     // Signal lower frequency
                        SIGLIB_ZERO,                // D.C. Offset
                        1.007,                      // Frequency change per sample period
                        SIGLIB_HALF,                // Signal upper frequency
                        &ChirpPhase,                // Chirp phase - used for next iteration
                        &ChirpValue,                // Chirp current value - used for next iteration
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Non linear chirp waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


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
                 SAMPLE_LENGTH,                     // Dataset length
                 "Non linear chirp spectrum",       // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nNon linear chirp spectrum\nPlease hit <Carriage Return> to continue . . ."); getchar ();


    ChirpPhase = SIGLIB_ZERO;
    ChirpValue = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                          // Output array pointer
                        SIGLIB_CHIRP_LIN,                   // Signal type - Chirp with linear frequency ramp
                        CHIRP_MAGNITUDE,                    // Signal peak level
                        SIGLIB_FILL,                        // Fill (overwrite) or add to existing array contents
                        (CHIRP_START_FREQ / SAMPLE_RATE),   // Signal lower frequency
                        SIGLIB_ZERO,                        // D.C. Offset
                        ((CHIRP_END_FREQ - CHIRP_START_FREQ) / (SAMPLE_LENGTH * SAMPLE_RATE)),  // Frequency change per sample period
                        (CHIRP_END_FREQ / SAMPLE_RATE),     // Signal upper frequency
                        &ChirpPhase,                        // Chirp phase - used for next iteration
                        &ChirpValue,                        // Chirp current value - used for next iteration
                        SAMPLE_LENGTH);                     // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Linear chirp waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                  SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();


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
                 SAMPLE_LENGTH,                     // Dataset length
                 "Linear chirp spectrum",           // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nLinear chirp spectrum\nPlease hit <Carriage Return> to continue . . ."); getchar ();


    GaussPhase = SIGLIB_ZERO;
    SDA_SignalGenerate (pRealData,                  // Output array pointer
                        SIGLIB_GAUSSIAN_NOISE,      // Signal type - Gaussian noise
                        SIGLIB_ZERO,                // Signal peak level - Unused
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        GAUS_NOISE_OFFSET,          // D.C. Offset
                        GAUS_NOISE_VARIANCE,        // Gaussian noise variance
                        SIGLIB_ZERO,                // Signal end value - Unused
                        &GaussPhase,                // Pointer to gaussian signal phase - should be initialised to zero
                        &GaussValue,                // Gaussian signal second sample - should be initialised to zero
                        SAMPLE_LENGTH);             // Output dataset length

    RMSValue = SDA_RootMeanSquare (pRealData, SAMPLE_LENGTH);

    sprintf (OutputString, "Gaussian noise waveform, RMS value = %lf", RMSValue);
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pRealData,                         // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 OutputString,                      // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\n%s\nPlease hit <Carriage Return> to continue . . .", (char *)OutputString); getchar ();

    SDA_Copy (pRealData, pResults, SAMPLE_LENGTH);
                                                    // Perform real FFT
    SDA_Rfft (pResults,                             // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

                                                    // Calculate real magnitude from complex
    SDA_Magnitude (pResults,                        // Pointer to real source array
                   pImagData,                       // Pointer to imaginary source array
                   pResults,                        // Pointer to magnitude destination array
                   FFT_LENGTH);                     // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Gaussian noise spectrum",         // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nGaussian noise spectrum\nPlease hit <Carriage Return> to continue . . ."); getchar ();


    SIF_Histogram (pResults, HGRAM_SAMPLE_LENGTH);
    SDA_Histogram (pRealData,                       // Input array pointer
                   pResults,                        // Output (histogram) array pointer
                   SIGLIB_HISTOGRAM_AUTOSCALE,      // Minimum input data value
                   SIGLIB_HISTOGRAM_AUTOSCALE,      // Maximum input data value
                   SAMPLE_LENGTH,                   // Input dataset length
                   HGRAM_SAMPLE_LENGTH);            // Output (histogram) dataset length


    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pResults,                          // Dataset
                 HGRAM_SAMPLE_LENGTH,               // Dataset length
                 "Gaussian noise histogram",        // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE), // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nGaussian noise histogram\n");
    printf ("Variance = %lf\n", SDA_UnbiasedVariance (pRealData, SAMPLE_LENGTH));

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pRealData);                     // Free memory
    SUF_MemoryFree (pImagData);
    SUF_MemoryFree (pResults);
    SUF_MemoryFree (pFFTCoeffs);
}


