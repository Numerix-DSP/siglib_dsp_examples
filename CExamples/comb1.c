// SigLib Comb Filter Example

// This program demonstrates the impulse and frequency response of four
// different arrangents of comb filter. The flow diagrams for each are :

// Comb filter type 1
//         |-----|     - |-----|
// IN ---->|Delay|------>|     |
//     |   |-----|       |     |
//     |               + |  +  |----> OUT
//     |---------------->|     |
                      // |-----|

// Comb filter type 2
//         |-----|
// IN ---->|comb |------> OUT
//         |-----|

// Comb filter type 3
//         |-----|      |-----|
// IN ---->|Comb1|----->|Comb2|-->|-----|
//     |   |-----|  |   |-----|   |     |
//     |            ------------->|  +  |----> OUT
//     |                          |     |
//     -------------------------->|-----|

// Comb filter type 4
//         |------|      |------|
// IN ---->|Delay1|----->|Delay2|-->|-----|
//     |   |------|  |   |------|   |     |
//     |             -------------->|  +  |----> OUT
//     |                            |     |
//     ---------------------------->|-----|

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C

// Define constants
#define SAMPLE_LENGTH   512
#define FFT_LENGTH      SAMPLE_LENGTH
#define LOG2_FFT_LENGTH 9

// Declare global variables and arrays
static SLArrayIndex_t   FilterLength;
static SLArrayIndex_t   FilterIndex1, FilterIndex2;
static SLData_t         FilterSum1, FilterSum2;
static SLData_t         *pSrc1, *pSrc2, *pSrc3, *pImagData, *pMagnitude, *pPhase, *pStateArray1, *pStateArray2, *pFFTCoeffs;
static SLData_t         K1, K2, K3;

void    main(void);

void main(void)
{
    h_GPC_Plot  *h2DPlot;                           // Plot object

    SLFixData_t i;

    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Comb Filter",                 // Plot title
                     "Time / Frequency",            // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    printf ("\n\n\nThis program demonstrates the impulse and frequency\n");
    printf ("response of four different arrangents of comb filter\n\n.");

    printf ("Enter a comb filter length < 512 =>");
    scanf ("%d", &FilterLength);
    printf ("\nComb filter length => %d\n", FilterLength);
    getchar ();                                     // Clear keyboard buffer

    pImagData = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pMagnitude = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pPhase = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pSrc1 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pSrc2 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pSrc3 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pStateArray1 = SUF_VectorArrayAllocate (FilterLength);
    pStateArray2 = SUF_VectorArrayAllocate (FilterLength);
    pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

                                                    // Initialise FFT
    SIF_Fft (pFFTCoeffs,                            // Pointer to FFT coefficients
             SIGLIB_NULL_ARRAY_INDEX_PTR,           // Pointer to bit reverse address table - NOT USED
             FFT_LENGTH);                           // FFT length

                            // Comb filter type 1
    printf ("\n\n\nComb filter type 1\n\n");
    printf ("        |-----|     -\n");
    printf ("IN ---->|Delay|------>|-----|\n");
    printf ("    |   |-----|       |     |\n");
    printf ("    |                 |     |\n");
    printf ("    |                 |  +  |----> OUT\n");
    printf ("    |                 |     |\n");
    printf ("    |               + |     |\n");
    printf ("    ----------------->|-----|\n");
    printf ("\n Comb filter length => %d", FilterLength);

    SIF_FixedDelay (pStateArray1,                   // Pointer to delay state array
                    &FilterIndex1,                  // Pointer to delay state index
                    FilterLength);                  // Delay length

                                    // Generate an impulse
    SDA_SignalGenerate (pSrc1,                      // Pointer to destination array
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

                                    // Apply comb filter and store filtered data
    for (i = 0; i < SAMPLE_LENGTH; i++) {
        *pSrc2++ =
            *pSrc1 - SDS_FixedDelay (*pSrc1,        // Input data sample
                                     pStateArray1,  // Pointer to delay state array
                                     &FilterIndex1, // Pointer to delay state index
                                     FilterLength); // Delay length
        pSrc1++;
    }

    pSrc2 -= SAMPLE_LENGTH;                         // Reset pointers
    pSrc1 -= SAMPLE_LENGTH;

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc2,                             // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Impulse Response",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nImpulse Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Rfft (pSrc2,                                // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length
    SDA_RectangularToPolar (pSrc2,                  // Pointer to source real array
                            pImagData,              // Pointer to source imaginary array
                            pMagnitude,             // Pointer to destination magnitude array
                            pPhase,                 // Pointer to destination phase array
                            SAMPLE_LENGTH);         // Dataset length
    SDA_Multiply (pMagnitude,                       // Pointer to source array
                  65536.,                           // Multiplier
                  pMagnitude,                       // Pointer to destination array
                  SAMPLE_LENGTH);                   // Dataset length
    SDA_10Log10 (pMagnitude,                        // Pointer to source array
                 pMagnitude,                        // Pointer to destination array
                 SAMPLE_LENGTH);                    // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pMagnitude,                        // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Magnitude  (dB)",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nMagnitude Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pPhase,                            // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Phase",                           // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_NEW);                          // New graph
    printf ("\nPhase Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();


                            // Comb filter type 2
    printf ("\n\n\nComb filter type 2\n\n");
    printf ("        |-----|\n");
    printf ("IN ---->|comb |------> OUT\n");
    printf ("        |-----|\n");
    printf ("\n Comb filter length => %d", FilterLength);

    SIF_Comb (pStateArray1,                         // Pointer to filter delay state array
              &FilterIndex1,                        // Pointer to filter index register
              &FilterSum1,                          // Pointer to filter sum register
              FilterLength);                        // Filter length

                                    // Generate an impulse
    SDA_SignalGenerate (pSrc1,                      // Pointer to destination array
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

            // Apply comb filter and store filtered data
    for (i = 0; i < SAMPLE_LENGTH; i++) {
        *pSrc2++ =
            SDS_Comb (*pSrc1++,                     // Input data sample to be filtered
                      pStateArray1,                 // Pointer to filter state array
                      &FilterIndex1,                // Pointer to filter index register
                      &FilterSum1,                  // Pointer to filter sum register
                      FilterLength);                // Filter length
    }

    pSrc2 -= SAMPLE_LENGTH;                         // Reset pointers
    pSrc1 -= SAMPLE_LENGTH;

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc2,                             // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Impulse Response",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nImpulse Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Rfft (pSrc2,                                // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

    SDA_RectangularToPolar (pSrc2,                  // Pointer to source real array
                            pImagData,              // Pointer to source imaginary array
                            pMagnitude,             // Pointer to destination magnitude array
                            pPhase,                 // Pointer to destination phase array
                            SAMPLE_LENGTH);         // Dataset length
    SDA_Multiply (pMagnitude,                       // Pointer to source array
                  65536.,                           // Multiplier
                  pMagnitude,                       // Pointer to destination array
                  SAMPLE_LENGTH);                   // Dataset length
    SDA_10Log10 (pMagnitude,                        // Pointer to source array
                 pMagnitude,                        // Pointer to destination array
                 SAMPLE_LENGTH);                    // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pMagnitude,                        // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Magnitude  (dB)",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nMagnitude Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pPhase,                            // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Phase",                           // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_NEW);                          // New graph
    printf ("\nPhase Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();


                            // Comb filter type 3
    printf ("\n\n\nComb filter type 3\n\n");
    printf ("        |-----|      |-----|\n");
    printf ("IN ---->|Comb1|----->|Comb2|-->|-----|\n");
    printf ("    |   |-----|  |   |-----|   |     |\n");
    printf ("    |            |             |     |\n");
    printf ("    |            ------------->|  +  |----> OUT\n");
    printf ("    |                          |     |\n");
    printf ("    |                          |     |\n");
    printf ("    -------------------------->|-----|\n");
    printf ("\n Comb filter length => %d", FilterLength);


    SIF_Comb (pStateArray1,                         // Pointer to filter delay state array
              &FilterIndex1,                        // Pointer to filter index register
              &FilterSum1,                          // Pointer to filter sum register
              FilterLength);                        // Filter length
    SIF_Comb (pStateArray2,                         // Pointer to filter delay state array
              &FilterIndex2,                        // Pointer to filter index register
              &FilterSum2,                          // Pointer to filter sum register
              FilterLength);                        // Filter length

                                                    // Generate an impulse
    SDA_SignalGenerate (pSrc1,                      // Pointer to destination array
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    for (i = 0; i < SAMPLE_LENGTH; i++) {
            // Apply comb filter 1 and store filtered data
        *pSrc2 =
            SDS_Comb (*pSrc1,                       // Input data sample to be filtered
                      pStateArray1,                 // Pointer to filter state array
                      &FilterIndex1,                // Pointer to filter index register
                      &FilterSum1,                  // Pointer to filter sum register
                      FilterLength);                // Filter length

            // Apply comb filter 1 and store filtered data
        *pSrc3 =
            SDS_Comb (*pSrc2,                       // Input data sample to be filtered
                      pStateArray2,                 // Pointer to filter state array
                      &FilterIndex2,                // Pointer to filter index register
                      &FilterSum2,                  // Pointer to filter sum register
                      FilterLength);                // Filter length

        *pSrc3 = *pSrc3 + *pSrc1++;
        *pSrc3 = *pSrc3 + *pSrc2++;
        pSrc3++;
    }

    pSrc1 -= SAMPLE_LENGTH;
    pSrc2 -= SAMPLE_LENGTH;
    pSrc3 -= SAMPLE_LENGTH;

    SDA_Scale (pSrc3,                               // Pointer to source array
               pSrc3,                               // Pointer to destination array
               SIGLIB_ONE,                          // Peak level
               SAMPLE_LENGTH);                      // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc3,                             // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Impulse Response",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nImpulse Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Rfft (pSrc3,                                // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

    SDA_RectangularToPolar (pSrc3,                  // Pointer to source real array
                            pImagData,              // Pointer to source imaginary array
                            pMagnitude,             // Pointer to destination magnitude array
                            pPhase,                 // Pointer to destination phase array
                            SAMPLE_LENGTH);         // Dataset length
    SDA_Multiply (pMagnitude,                       // Pointer to source array
                  65536.,                           // Multiplier
                  pMagnitude,                       // Pointer to destination array
                  SAMPLE_LENGTH);                   // Dataset length
    SDA_10Log10 (pMagnitude,                        // Pointer to source array
                 pMagnitude,                        // Pointer to destination array
                 SAMPLE_LENGTH);                    // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pMagnitude,                        // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Magnitude  (dB)",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nMagnitude Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pPhase,                            // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Phase",                           // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_NEW);                          // New graph
    printf ("\nPhase Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();



                            // Comb filter type 4
    printf ("\n\n\nComb filter type 4\n");
    printf ("        |------|      |------|\n");
    printf ("IN ---->|Delay1|----->|Delay2|-->|-----|\n");
    printf ("    |   |------|  |   |------|   |     |\n");
    printf ("    |             |              |     |\n");
    printf ("    |             -------------->|  +  |----> OUT\n");
    printf ("    |                            |     |\n");
    printf ("    |                            |     |\n");
    printf ("    ---------------------------->|-----|\n");
    printf ("\n Comb filter length => %d", FilterLength);

                        // Initialize delays
    SIF_FixedDelay (pStateArray1,                   // Pointer to delay state array
                    &FilterIndex1,                  // Pointer to delay state index
                    FilterLength);                  // Delay length
    SIF_FixedDelay (pStateArray2,                   // Pointer to delay state array
                    &FilterIndex2,                  // Pointer to delay state index
                    FilterLength);                  // Delay length

    K1 = SIGLIB_QUARTER;
    K2 = SIGLIB_MINUS_HALF;
    K3 = SIGLIB_QUARTER;

//    K1 = SIGLIB_HALF;
//    K2 = SIGLIB_HALF;
//    K3 = SIGLIB_ZERO;

                                    // Generate an impulse
    SDA_SignalGenerate (pSrc1,                      // Pointer to destination array
                        SIGLIB_IMPULSE,             // Signal type - Impulse function
                        SIGLIB_ONE,                 // Signal peak level
                        SIGLIB_FILL,                // Fill (overwrite) or add to existing array contents
                        SIGLIB_ZERO,                // Signal frequency - Unused
                        SIGLIB_ZERO,                // D.C. Offset
                        SIGLIB_ZERO,                // Delay (samples periods) applied to impulse
                        SIGLIB_ZERO,                // Signal end value - Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SIGLIB_NULL_DATA_PTR,       // Unused
                        SAMPLE_LENGTH);             // Output dataset length

    for (i = 0; i < SAMPLE_LENGTH; i++) {
            // Apply comb filter 1 and store filtered data
        *pSrc2 = SDS_FixedDelay (*pSrc1,            // Input data sample
                                 pStateArray1,      // Pointer to delay state array
                                 &FilterIndex1,     // Pointer to delay state index
                                 FilterLength);     // Delay length

            // Apply comb filter 1 and store filtered data
        *pSrc3 = K3 * SDS_FixedDelay (*pSrc2,           // Input data sample
                                      pStateArray2,     // Pointer to delay state array
                                      &FilterIndex2,    // Pointer to delay state index
                                      FilterLength);    // Delay length

        *pSrc3 = *pSrc3 + (*pSrc1++ * K1);
        *pSrc3 = *pSrc3 + (*pSrc2++ * K2);
        pSrc3++;
    }

    pSrc1 -= SAMPLE_LENGTH;
    pSrc2 -= SAMPLE_LENGTH;
    pSrc3 -= SAMPLE_LENGTH;

    SDA_Scale (pSrc3,                               // Pointer to source array
               pSrc3,                               // Pointer to destination array
               SIGLIB_ONE,                          // Peak level
               SAMPLE_LENGTH);                      // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pSrc3,                             // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Impulse Response",                // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nImpulse Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

                                                    // Perform real FFT
    SDA_Rfft (pSrc3,                                // Pointer to real array
              pImagData,                            // Pointer to imaginary array
              pFFTCoeffs,                           // Pointer to FFT coefficients
              SIGLIB_NULL_ARRAY_INDEX_PTR,          // Pointer to bit reverse address table - NOT USED
              FFT_LENGTH,                           // FFT length
              LOG2_FFT_LENGTH);                     // log2 FFT length

    SDA_RectangularToPolar (pSrc3,                  // Pointer to source real array
                            pImagData,              // Pointer to source imaginary array
                            pMagnitude,             // Pointer to destination magnitude array
                            pPhase,                 // Pointer to destination phase array
                            SAMPLE_LENGTH);         // Dataset length
    SDA_Multiply (pMagnitude,                       // Pointer to source array
                  65536.,                           // Multiplier
                  pMagnitude,                       // Pointer to destination array
                  SAMPLE_LENGTH);                   // Dataset length
    SDA_10Log10 (pMagnitude,                        // Pointer to source array
                 pMagnitude,                        // Pointer to destination array
                 SAMPLE_LENGTH);                    // Dataset length

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pMagnitude,                        // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Magnitude  (dB)",                 // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("\nMagnitude Response\nPlease hit <Carriage Return> to continue . . ."); getchar ();

    gpc_plot_2d (h2DPlot,                           // Graph handle
                 pPhase,                            // Dataset
                 SAMPLE_LENGTH,                     // Dataset length
                 "Phase",                           // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(SAMPLE_LENGTH - 1),       // Maximum X value
                 "lines",                           // Graph type
                 "red",                             // Colour
                 GPC_NEW);                          // New graph
    printf ("\nPhase Response\n");

    printf ("\nHit <Carriage Return> to continue ....\n"); getchar (); // Wait for <Carriage Return>
    gpc_close (h2DPlot);

    SUF_MemoryFree (pImagData);                     // Free all memory
    SUF_MemoryFree (pMagnitude);
    SUF_MemoryFree (pPhase);
    SUF_MemoryFree (pSrc1);
    SUF_MemoryFree (pSrc2);
    SUF_MemoryFree (pSrc3);
    SUF_MemoryFree (pStateArray1);
    SUF_MemoryFree (pStateArray2);
    SUF_MemoryFree (pFFTCoeffs);
}


