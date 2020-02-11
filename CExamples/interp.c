// SigLib Interpolation Functions Examples
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define DISPLAY_SINC_LUT    1                       // Set to '1' to display the Sinc Look Up Table
#define INPUT_LEN           8

// Declare global variables and arrays
static const SLData_t   InputX[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
static const SLData_t   InputY[] = {0.0, 0.7071, 1.0, 0.7071, 0.0, -0.7071, -1.0, -0.7071};

                                                    // Parameters for quick sinc look up table
#define NUMBER_OF_SINC_SIDELOBES    2               // Number of sinc sidelobes
#define SINC_LUT_LENGTH             512
#define MAX_SINC_INPUT_MAGNITUDE    ((11. * SIGLIB_PI) / 4.)

static SLData_t     LookUpTablePhaseGain;
static SLData_t     SincLUT [SINC_LUT_LENGTH];

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object


    printf ("Single array linear interpolation\n");
    printf ("x = %1.3lf, y = %1.3lf\n", 1.143,
            SDA_InterpolateLinear1 (InputY,
                                    1.143,
                                    8));
    printf ("x = %1.3lf, y = %1.3lf\n", 2.286,
            SDA_InterpolateLinear1 (InputY,         // Pointer to Y source array
                                    2.286,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 3.429,
            SDA_InterpolateLinear1 (InputY,         // Pointer to Y source array
                                    3.429,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 4.571,
            SDA_InterpolateLinear1 (InputY,         // Pointer to Y source array
                                    4.571,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 5.714,
            SDA_InterpolateLinear1 (InputY,         // Pointer to Y source array
                                    5.714,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 6.857,
            SDA_InterpolateLinear1 (InputY,         // Pointer to Y source array
                                    6.857,          // Input x value
                                    8));            // Input dataset length
            // This should generate a ZERO because it is beyond the input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 8.0,
            SDA_InterpolateLinear1 (InputY,         // Pointer to Y source array
                                    8.0,            // Input x value
                                    8));            // Input dataset length

    printf ("Dual array linear interpolation\n");
    printf ("x = %1.3lf, y = %1.3lf\n", 1.143,
            SDA_InterpolateLinear2 (InputX,         // Pointer to X source array
                                    InputY,         // Pointer to Y source array
                                    1.143,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 2.286,
            SDA_InterpolateLinear2 (InputX,         // Pointer to X source array
                                    InputY,         // Pointer to Y source array
                                    2.286,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 3.429,
            SDA_InterpolateLinear2 (InputX,         // Pointer to X source array
                                    InputY,         // Pointer to Y source array
                                    3.429,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 4.571,
            SDA_InterpolateLinear2 (InputX,         // Pointer to X source array
                                    InputY,         // Pointer to Y source array
                                    4.571,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 5.714,
            SDA_InterpolateLinear2 (InputX,         // Pointer to X source array
                                    InputY,         // Pointer to Y source array
                                    5.714,          // Input x value
                                    8));            // Input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 6.857,
            SDA_InterpolateLinear2 (InputX,         // Pointer to X source array
                                    InputY,         // Pointer to Y source array
                                    6.857,          // Input x value
                                    8));            // Input dataset length
            // This should generate a ZERO because it is beyond the input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 8.0,        // Pointer to X source array
            SDA_InterpolateLinear2 (InputX,         // Pointer to Y source array
                                    InputY,         // Input x value
                                    8.0,            // Input dataset length
                                    8));

                                                    // Initialise sinc re-sampling function
    SIF_InterpolateSinc1 (SincLUT,                  // Pointer to sinc LUT array
                          &LookUpTablePhaseGain,    // Pointer to phase gain
                          NUMBER_OF_SINC_SIDELOBES, // Number of sinc sidelobes
                          SINC_LUT_LENGTH);         // Look up table length

#if DISPLAY_SINC_LUT
    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Interpolation Functions",     // Plot title
                     "Time",                        // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 SincLUT,                           // Dataset
                 SINC_LUT_LENGTH,                   // Dataset length
                 "Sinc Look Up Table",              // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SINC_LUT_LENGTH - 1),     // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nSinc Look Up Table\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);
#endif

    printf ("Single array sinc interpolation\n");
    printf ("x = %1.3lf, y = %1.3lf\n", 1.143,
            SDA_InterpolateSinc1 (InputY,                   // Pointer to Y source array
                                  1.143,                    // Input x value
                                  SincLUT,                  // Pointer to LUT array
                                  LookUpTablePhaseGain,     // Look up table phase gain
                                  NUMBER_OF_SINC_SIDELOBES, // Number of adjacent samples
                                  8));                      // Source dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 2.286,
            SDA_InterpolateSinc1 (InputY,                   // Pointer to Y source array
                                  2.286,                    // Input x value
                                  SincLUT,                  // Pointer to LUT array
                                  LookUpTablePhaseGain,     // Look up table phase gain
                                  NUMBER_OF_SINC_SIDELOBES, // Number of adjacent samples
                                  8));                      // Source dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 3.429,
            SDA_InterpolateSinc1 (InputY,                   // Pointer to Y source array
                                  3.429,                    // Input x value
                                  SincLUT,                  // Pointer to LUT array
                                  LookUpTablePhaseGain,     // Look up table phase gain
                                  NUMBER_OF_SINC_SIDELOBES, // Number of adjacent samples
                                  8));                      // Source dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 4.571,
            SDA_InterpolateSinc1 (InputY,                   // Pointer to Y source array
                                  4.571,                    // Input x value
                                  SincLUT,                  // Pointer to LUT array
                                  LookUpTablePhaseGain,     // Look up table phase gain
                                  NUMBER_OF_SINC_SIDELOBES, // Number of adjacent samples
                                  8));                      // Source dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 5.714,
            SDA_InterpolateSinc1 (InputY,                   // Pointer to Y source array
                                  5.714,                    // Input x value
                                  SincLUT,                  // Pointer to LUT array
                                  LookUpTablePhaseGain,     // Look up table phase gain
                                  NUMBER_OF_SINC_SIDELOBES, // Number of adjacent samples
                                  8));                      // Source dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 6.857,
            SDA_InterpolateSinc1 (InputY,                   // Pointer to Y source array
                                  6.857,                    // Input x value
                                  SincLUT,                  // Pointer to LUT array
                                  LookUpTablePhaseGain,     // Look up table phase gain
                                  NUMBER_OF_SINC_SIDELOBES, // Number of adjacent samples
                                  8));                      // Source dataset length
            // This should generate a ZERO because it is beyond the input dataset length
    printf ("x = %1.3lf, y = %1.3lf\n", 10.0,
            SDA_InterpolateSinc1 (InputY,                   // Pointer to Y source array
                                  10.0,                     // Input x value
                                  SincLUT,                  // Pointer to LUT array
                                  LookUpTablePhaseGain,     // Look up table phase gain
                                  NUMBER_OF_SINC_SIDELOBES, // Number of adjacent samples
                                  8));                      // Source dataset length
}


