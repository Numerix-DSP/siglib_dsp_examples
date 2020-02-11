// SigLib Cornu's Spiral Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH   2048                        // # samples
#define AMPLITUDE       8.                          // # sine cycles
#define OFFSET          SIGLIB_ZERO                 // Ramp offset

// Declare global variables and arrays
static SLData_t         *pData1, *pData2;           // Dataset pointers
static SLComplexRect_s  *pComplexData;
static SLData_t         IntegralSum1, IntegralSum2;
static SLData_t         RampPhase;

void main(void)
{
    h_GPC_Plot  *hXYGraph;                          // Plot object

                                                    // Allocate memory
    pData1 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pData2 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pComplexData = SUF_ComplexRectArrayAllocate (SAMPLE_LENGTH);

    hXYGraph =                                      // Initialize plot
        gpc_init_xy ("Cornu's Spiral",              // Plot title
                     "X-Axis",                      // X-Axis label
                     "Y-Axis",                      // Y-Axis label
                     (float)1.1,                    // Dimension - this is square
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (hXYGraph == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }


    RampPhase = SIGLIB_ZERO;
    SDA_SignalGenerateRamp  (pData1,                // Pointer to destination array
                             AMPLITUDE,             // Amplitude
                             OFFSET,                // D.C. Offset
                             &RampPhase,            // Phase - maintained across array boundaries
                             SAMPLE_LENGTH);        // Dataset length

    SDA_Power (pData1,                              // Pointer to source array
               pData1,                              // Pointer to destination array
               2,                                   // Power to raise souce data by
               SAMPLE_LENGTH);                      // Dataset length

    SDA_Sin (pData1,                                // Pointer to source array
             pData2,                                // Pointer to destination array
             SAMPLE_LENGTH);                        // Dataset length
    SDA_Cos (pData1,                                // Pointer to source array
             pData1,                                // Pointer to destination array
             SAMPLE_LENGTH);                        // Dataset length

    IntegralSum1 = SIGLIB_ZERO;
    IntegralSum2 = SIGLIB_ZERO;

    SDA_Integrate (pData1,                          // Pointer to source array
                   pData1,                          // Pointer to destination array
                   10000.,                          // Maximum value for integral
                   SIGLIB_ONE,                      // Integral decay value - no decay
                   &IntegralSum1,                   // Internal integral sum - maintained across array boundaries
                   SAMPLE_LENGTH);                  // Dataset length
    SDA_Integrate (pData2,                          // Pointer to source array
                   pData2,                          // Pointer to destination array
                   10000.,                          // Maximum value for integral
                   SIGLIB_ONE,                      // Integral decay value - no decay
                   &IntegralSum2,                   // Internal integral sum - maintained across array boundaries
                   SAMPLE_LENGTH);                  // Dataset length

                                                    // Scale results so peaks equal 1.0
    SDA_Scale (pData1,                              // Pointer to source array
               pData1,                              // Pointer to destination array
               SIGLIB_ONE,                          // Peak level
               SAMPLE_LENGTH);                      // Dataset length
    SDA_Scale (pData2,                              // Pointer to source array
               pData2,                              // Pointer to destination array
               SIGLIB_ONE,                          // Peak level
               SAMPLE_LENGTH);                      // Dataset length

    SDA_CreateComplexRect (pData1,                  // Pointer to real source array
                           pData2,                  // Pointer to imaginary source array
                           pComplexData,            // Pointer to complex destination array
                           SAMPLE_LENGTH);          // Dataset length

    gpc_plot_xy (hXYGraph,                          // Graph handle
                 (ComplexRect_s *)pComplexData,     // Array of complex dataset
                 (int)SAMPLE_LENGTH,                // Dataset length
                 "Cornu's Spiral",                  // Dataset title
                 "points pt 7 ps 0.5",              // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph

    printf ("Cornu's Spiral\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (hXYGraph);

    SUF_MemoryFree (pData1);                        // Free memory
    SUF_MemoryFree (pData2);
    SUF_MemoryFree (pComplexData);

}       // End of main()

