// SigLib example for pi/4 Differential Quadrature Phase Shift keying (DQPSK) modulation
//     Data rate - 2400 bps
//     Baud rate - 1200 Baud
//     Sample rate - 48000 Hz
//     Carrier freq. - 1800 Hz

// Include files
#include <stdio.h>
#include <string.h>
#include <siglib.h>                                 // SigLib DSP library
#include <gnuplot_c.h>                              // Gnuplot/C
#include <nhl.h>                                    // Numerix host library
#include "Tx_FIR.h"                                 // Transmitter FIR low-pass filter

// Define constants
#define RRCF_ENABLE                     0           // Root raised cosine filter on Tx and Rx

                    // Select one of the following three
#define DISPLAY_TIME_DOMAIN             0           // Time domain output
#define DEBUG_LOG_FILE                  0           // Set to '1' to enable logging to debug.log

                    // Basic application definitions
// #define SAMPLE_LENGTH                   512                 // Number of samples in array
#define SAMPLE_LENGTH                   480                 // Number of samples in array

#define SAMPLE_RATE                     48000.              // Sample rate
#define SYMBOL_RATE                     1200.               // Symbol rate
#define CARRIER_TABLE_FREQ              100.                // Frequency of sine wave in table
#define CARRIER_FREQ                    1800.               // Frequency of carrier signal - a multiple of the sine table frequency

#define NOISE_LENGTH                    ((SAMPLE_LENGTH * 15) + 100)    // Length of the noise sections

#if RRCF_ENABLE
#define RRCF_PERIOD                     (SAMPLE_RATE / SYMBOL_RATE) // RRCF Period
#define RRCF_ROLL_OFF                   0.75                // Root raised cosine filter roll off factor
#define RRCF_LENGTH                     ((3 * SYMBOL_LENGTH) | 0x01)    // Root raised cosine filter length
#else
#define RRCF_PERIOD                     SIGLIB_ZERO         // Dummy value - RRCF Period
#define RRCF_ROLL_OFF                   SIGLIB_ZERO         // Dummy value - Root raised cosine filter roll off factor
#define RRCF_LENGTH                     1                   // Dummy value - Root raised cosine filter length
#endif

                                                            // Injected noise parameters
// #define GAUS_NOISE_VARIANCE             4.0
#define GAUS_NOISE_VARIANCE             0.01
// #define GAUS_NOISE_VARIANCE             0.0
#define GAUS_NOISE_OFFSET               SIGLIB_ZERO


            // Derived application definitions
#define SYMBOL_LENGTH                   40 //((SLArrayIndex_t)(SAMPLE_RATE / SYMBOL_RATE))           // Number of samples per symbol
#define SYMBOLS_PER_LOOP                12 //((SLArrayIndex_t)(SAMPLE_LENGTH / SYMBOL_LENGTH))       // Number of symbols per loop for graph

#define CARRIER_SINE_TABLE_SIZE         480 //((SLArrayIndex_t)(SAMPLE_RATE / CARRIER_TABLE_FREQ))    // Number of samples in each of cos and sine table
#define CARRIER_TABLE_INCREMENT         18 //((SLArrayIndex_t)(CARRIER_FREQ / CARRIER_TABLE_FREQ))   // Carrier frequency

// Declare global variables and arrays

static SLData_t         *pCarrierTable;                     // Overlapped cosine + sine look-up table

                                                            // Modem parameters and variables
static SLData_t         TxCarrierPhase;
static SLArrayIndex_t   TxPreviousOutputSymbol;

                            // RRCF arrays
static SLData_t         TxIFilterState[RRCF_LENGTH];
static SLData_t         TxQFilterState[RRCF_LENGTH];
static SLData_t         RRCFCoeffs[RRCF_LENGTH];            // Raised cosine filter coefficients
static SLArrayIndex_t   TxIFilterIndex;
static SLArrayIndex_t   TxQFilterIndex;

static SLArrayIndex_t   TxSampleClock;                      // Used to keep track of the samples and symbols
static SLComplexRect_s  TxMagn;                             // Used to calculate the signal magnitude


static SLData_t         OutputArray[SAMPLE_LENGTH];

static SLData_t         GaussianNoisePhase, GaussianNoiseValue; // Variables for injecting noise

static WAV_FILE_INFO    WavInfo;
static SLFixData_t      TxDiBit;

#if DISPLAY_TIME_DOMAIN
    static h_GPC_Plot   *h2DPlot;                           // Plot object
#endif

void inject_noise (FILE *);

void main (void);

void main (void)

{
    FILE            *fpi, *fpo;
    int             InputChar;
    int             TotalSampleCount = 0;

    pCarrierTable = SUF_QPSKCarrierArrayAllocate (CARRIER_SINE_TABLE_SIZE); // Allocate arrays

                                                    // Initialize FIR low-pass filter
    SIF_Fir (TxPreFilterState,                      // Pointer to filter state array
             &TxPreFilterIndex,                     // Pointer to filter index register
             TX_PRE_FILTER_LENGTH);                 // Filter length

#if DISPLAY_TIME_DOMAIN
    h2DPlot =                                       // Initialize plot
        gpc_init_2d ("Pi/4 DQPSK Modulator",        // Plot title
                     "Time",                        // X-Axis label
                     "Magnitude",                   // Y-Axis label
                     GPC_AUTO_SCALE,                // Scaling mode
                     GPC_SIGNED,                    // Sign mode
                     GPC_KEY_ENABLE);               // Legend / key mode

    if (h2DPlot == NULL) {                          // Graph creation failed - e.g is server running ?
        printf ("Graph creation failure. Please check that the server is running\n");
        exit (1);
    }
#endif
#if DEBUG_LOG_FILE
    SUF_ClearDebugfprintf();
#endif

                                                    // Initialise QPSK functions
    SIF_PiByFourDQpskModulate (pCarrierTable,                       // Carrier table pointer
                               CARRIER_TABLE_FREQ / SAMPLE_RATE,    // Carrier frequency
                               CARRIER_SINE_TABLE_SIZE,             // Carrier sine table size
                               &TxCarrierPhase,                     // Carrier phase pointer
                               &TxSampleClock,                      // Sample clock pointer
                               &TxMagn,                             // Magnitude pointer
                               TxIFilterState,                      // RRCF Tx I delay pointer
                               &TxIFilterIndex,                     // RRCF Tx I Filter Index pointer
                               TxQFilterState,                      // RRCF Tx Q delay pointer
                               &TxQFilterIndex,                     // RRCF Tx Q Filter Index pointer
                               RRCFCoeffs,                          // RRCF Coefficients pointer
                               RRCF_PERIOD,                         // RRCF Period
                               RRCF_ROLL_OFF,                       // RRCF Roll off
                               RRCF_LENGTH,                         // RRCF size
                               RRCF_ENABLE,                         // RRCF enable / disable switch
                               &TxPreviousOutputSymbol);            // Pointer to previous output symbol for differential coding

    if ((fpi = fopen("base.txt", "r")) == NULL) {
        printf ("Error opening input file %s\n", "base.txt");
        exit (1);
    }
    if ((fpo = fopen("base.wav", "wb")) == NULL) {  // Note this file is binary
        printf ("Error opening input file %s\n", "base.wav");
        exit (1);
    }

    WavInfo = wav_set_info (SAMPLE_RATE, 0, (short)1, (short)16, (short)2, (short)1);
    wav_write_header (fpo, WavInfo);                // Write the header information

    GaussianNoisePhase = SIGLIB_ZERO;

    inject_noise (fpo); TotalSampleCount += NOISE_LENGTH;  // Inject noise into the output, if required

    while ((InputChar = getc (fpi)) != EOF) {       // Get first bit
        if (InputChar == '\n') {
            inject_noise (fpo); TotalSampleCount += NOISE_LENGTH;   // Inject noise into the output, if required
        }
        else {
            if (InputChar == '1') {                 // Convert di-bit pair from input file
                TxDiBit = 0x2;
            }
            else if (InputChar == '0') {
                TxDiBit = 0x0;
            }

            if ((InputChar = getc (fpi)) == EOF) {  // Get second bit
                                                    // No second bit so exiting
                inject_noise (fpo); TotalSampleCount += NOISE_LENGTH;   // Inject noise into the output, if required

#if DISPLAY_TIME_DOMAIN
                gpc_close (h2DPlot);
#endif
                SUF_MemoryFree (pCarrierTable);             // Free memory

                WavInfo.NumberOfSamples = TotalSampleCount; // Set total data length
                rewind (fpo);                               // Rewind pointer to start of file
                wav_write_header (fpo, WavInfo);            // Overwrite the header information

                printf ("Total Tx Sample Count = %d\n", TotalSampleCount);

                fclose (fpi);
                fclose (fpo);
                exit (0);
            }

            if (InputChar == '\n') {
                inject_noise (fpo); TotalSampleCount += NOISE_LENGTH;   // Inject noise into the output, if required
            }

            else {
                if (InputChar == '1')   TxDiBit += 0x1;         // Convert di-bit pair from input file

                SDA_PiByFourDQpskModulate (TxDiBit,                     // Source data di-bit
                                           OutputArray,                 // Destination array
                                           pCarrierTable,               // Carrier table pointer
                                           CARRIER_SINE_TABLE_SIZE,     // Carrier sine table size
                                           &TxCarrierPhase,             // Carrier phase pointer
                                           &TxSampleClock,              // Sample clock pointer
                                           &TxMagn,                     // Magnitude pointer
                                           CARRIER_TABLE_INCREMENT,     // Carrier table increment
                                           SYMBOL_LENGTH,               // Samples per symbol
                                           TxIFilterState,              // RRCF Tx I delay pointer
                                           &TxIFilterIndex,             // RRCF Tx I Filter Index pointer
                                           TxQFilterState,              // RRCF Tx Q delay pointer
                                           &TxQFilterIndex,             // RRCF Tx Q Filter Index pointer
                                           RRCFCoeffs,                  // RRCF Coefficients pointer
                                           RRCF_LENGTH,                 // RRCF size
                                           RRCF_ENABLE,                 // RRCF enable / disable switch
                                           &TxPreviousOutputSymbol);    // Pointer to previous output symbol for differential coding

                                                            // Set the output signal magnitude
                SDA_Multiply (OutputArray,                  // Pointer to source array
                              100.0,                        // Multiplier
                              OutputArray,                  // Pointer to destination array
                              SYMBOL_LENGTH);               // Dataset length

                SDA_Fir (OutputArray,                       // Input array to be filtered
                         OutputArray,                       // Filtered output array
                         TxPreFilterState,                  // Pointer to filter state array
                         TxPreFilterCoeffs,                 // Pointer to filter coefficients
                         &TxPreFilterIndex,                 // Pointer to filter index register
                         TX_PRE_FILTER_LENGTH,              // Filter length
                         SYMBOL_LENGTH);                    // Dataset length

                wav_write_data (OutputArray, fpo, WavInfo, SYMBOL_LENGTH);
                TotalSampleCount += SYMBOL_LENGTH;

#if DISPLAY_TIME_DOMAIN
                gpc_plot_2d (h2DPlot,                       // Graph handle
                             OutputArray,                   // Dataset
                             SYMBOL_LENGTH,                 // Dataset length
                             "Output data",                 // Dataset title
                             SIGLIB_ZERO,                   // Minimum X value
                             (double)(SYMBOL_LENGTH - 1),   // Maximum X value
                             "lines",                       // Graph type
                             "blue",                        // Colour
                             GPC_NEW);                      // New graph
                printf ("Please hit <Carriage Return> to continue . . ."); getchar ();
#endif
            }
        }
    }

    inject_noise (fpo); TotalSampleCount += NOISE_LENGTH;   // Inject noise into the output, if required

#if DISPLAY_TIME_DOMAIN
    gpc_close (h2DPlot);
#endif
    SUF_MemoryFree (pCarrierTable);                 // Free memory

    WavInfo.NumberOfSamples = TotalSampleCount;     // Set total data length
    wav_write_header (fpo, WavInfo);                // Overwrite the header information

    fclose (fpi);                                   // Close files
    fclose (fpo);
}


void inject_noise (FILE *fpo)

{
    int j;

    for (j = 0; j < 15; j++) {
                                                    // Pre fill with noise
        SDA_SignalGenerate (OutputArray,            // Pointer to destination array
                            SIGLIB_GAUSSIAN_NOISE,  // Signal type
                            SIGLIB_ZERO,            // Peak value of signal
                            SIGLIB_FILL,            // Buffer fill mode, fill up or add to
                            SIGLIB_ZERO,            // Signal frequency
                            GAUS_NOISE_OFFSET,      // Signal offset
                            GAUS_NOISE_VARIANCE,    // Param, different for each signal type
                            SIGLIB_ZERO,            // End value
                            &GaussianNoisePhase,    // Phase offset
                            &GaussianNoiseValue,    // Current value
                            SAMPLE_LENGTH);         // Dataset length

#if DISPLAY_TIME_DOMAIN
        gpc_plot_2d (h2DPlot,                       // Graph handle
                     OutputArray,                   // Dataset
                     SAMPLE_LENGTH,                 // Dataset length
                     "Output data",                 // Dataset title
                     SIGLIB_ZERO,                   // Minimum X value
                     (double)(SAMPLE_LENGTH - 1),   // Maximum X value
                     "lines",                       // Graph type
                     "blue",                        // Colour
                     GPC_NEW);                      // New graph
        printf ("Please hit <Carriage Return> to continue . . ."); getchar ();
#endif
        SDA_Fir (OutputArray,                       // Input array to be filtered
                 OutputArray,                       // Filtered output array
                 TxPreFilterState,                  // Pointer to filter state array
                 TxPreFilterCoeffs,                 // Pointer to filter coefficients
                 &TxPreFilterIndex,                 // Pointer to filter index register
                 TX_PRE_FILTER_LENGTH,              // Filter length
                 SYMBOL_LENGTH);                    // Dataset length

        wav_write_data (OutputArray, fpo, WavInfo, SAMPLE_LENGTH);
    }

    SDA_SignalGenerate (OutputArray,                // Pointer to destination array
                        SIGLIB_GAUSSIAN_NOISE,      // Signal type
                        SIGLIB_ZERO,                // Peak value of signal
                        SIGLIB_FILL,                // Buffer fill mode, fill up or add to
                        SIGLIB_ZERO,                // Signal frequency
                        GAUS_NOISE_OFFSET,          // Signal offset
                        GAUS_NOISE_VARIANCE,        // Param, different for each signal type
                        SIGLIB_ZERO,                // End value
                        &GaussianNoisePhase,        // Phase offset
                        &GaussianNoiseValue,        // Current value
                        SAMPLE_LENGTH);             // Dataset length

#if DISPLAY_TIME_DOMAIN
    gpc_plot_2d (h2DPlot,                           // Graph handle
                 OutputArray,                       // Dataset
                 100,                               // Dataset length
                 "Output data",                     // Dataset title
                 SIGLIB_ZERO,                       // Minimum X value
                 (double)(99),                      // Maximum X value
                 "lines",                           // Graph type
                 "blue",                            // Colour
                 GPC_NEW);                          // New graph
    printf ("Please hit <Carriage Return> to continue . . ."); getchar ();
#endif

    SDA_Fir (OutputArray,                           // Input array to be filtered
             OutputArray,                           // Filtered output array
             TxPreFilterState,                      // Pointer to filter state array
             TxPreFilterCoeffs,                     // Pointer to filter coefficients
             &TxPreFilterIndex,                     // Pointer to filter index register
             TX_PRE_FILTER_LENGTH,                  // Filter length
             SYMBOL_LENGTH);                        // Dataset length

    wav_write_data (OutputArray, fpo, WavInfo, 100);
}


