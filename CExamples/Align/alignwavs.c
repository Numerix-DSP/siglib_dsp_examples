// SigLib - .WAV file alignment and display program
// Copyright (C) 2020 Sigma Numerix Ltd.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <siglib_host_utils.h>                      // Optionally includes conio.h and time.h subset functions
#include <math.h>
#include <siglib.h>                                 // SigLib DSP library
#include <nhl.h>                                    // Numerix host library
#include <gnuplot_c.h>                              // Gnuplot/C

static double  *pDataArray1, *pDataArray2;
static double  *pAlignedArray1, *pAlignedArray2;
static char    Filename1[80];
static char    Filename2[80];

static WAV_FILE_INFO WavInfo1, WavInfo2;


void main (int argc, char *argv[]);

void main (int argc, char *argv[])
{
    h_GPC_Plot  *h2DPlot;                                   // Plot object

    SLArrayIndex_t  outputLength;

    if (argc != 3) {
        printf ("\nUsage error  :\nalignwav Filename1 Filename2\n\n");
        exit (1);                                           // Exit - usage error
    }

    strcpy (Filename1, argv[1]);
    strcpy (Filename2, argv[2]);

    printf ("Source file 1 = %s\n", Filename1);
    printf ("Source file 2 = %s\n", Filename2);

    printf ("\nAligning data arrays. This may take a little time ...\n");

    pDataArray1 = SUF_VectorArrayAllocate (wav_file_length(Filename1));        // Input data arrays
    pDataArray2 = SUF_VectorArrayAllocate (wav_file_length(Filename2));

    pAlignedArray1 = SUF_VectorArrayAllocate (wav_file_length(Filename1) + wav_file_length(Filename2) - 1); // Aligned data arrays
    pAlignedArray2 = SUF_VectorArrayAllocate (wav_file_length(Filename1) + wav_file_length(Filename2) - 1);

    WavInfo1 = wav_read_file (pDataArray1, Filename1);
    if (WavInfo1.NumberOfSamples == -1) {
        printf ("Error reading .WAV file header : %s\n", Filename1);
        exit (1);
    }

    WavInfo2 = wav_read_file (pDataArray2, Filename2);
    if (WavInfo2.NumberOfSamples == -1) {
        printf ("Error reading .WAV file header : %s\n", Filename2);
        exit (1);
    }

    outputLength =
        SDA_Align (pDataArray1,                         // Pointer to input array #1
                   pDataArray2,                         // Pointer to input array #2
                   pAlignedArray1,                      // Pointer to destination array #1
                   pAlignedArray2,                      // Pointer to destination array #2
                   SIGLIB_ALIGN_CROP,                   // Alignment mode
                   WavInfo1.NumberOfSamples,            // Length of array #1
                   WavInfo2.NumberOfSamples);           // Length of array #2

    printf ("Data File #1 read    = %d samples\n", WavInfo1.NumberOfSamples);
    printf ("Data File #2 read    = %d samples\n", WavInfo2.NumberOfSamples);
    printf ("Output Array Lengths = %d samples\n", outputLength);

    wav_write_file (pAlignedArray1, "aligned_1.wav", WavInfo1, outputLength);
    wav_write_file (pAlignedArray2, "aligned_2.wav", WavInfo2, outputLength);

    h2DPlot =                                       // Initialize plot
        gpc_init_2d_dual_plot ("Plot wave",         // Plot title
                               GPC_KEY_ENABLE);     // Legend / key mode
    if (h2DPlot == NULL) {
        printf ("\nPlot creation failure.\n");
        exit (1);
    }

    gpc_plot_2d_dual_plot (h2DPlot,                     // Graph handle
                           "Time (sec)",                // X-Axis label
                           SIGLIB_ZERO,                 // Minimum X value
                           (double)(outputLength / WavInfo1.SampleRate),    // Maximum X value
                           pAlignedArray1,              // Dataset #1
                           "Aligned Signal #1",         // Dataset #1 title
                           "lines",                     // Graph type #1
                           "blue",                      // Colour #1
                           "Magnitude",                 // Y-Axis label #1
                           GPC_AUTO_SCALE,              // Scaling mode #1
                           GPC_SIGNED,                  // Sign mode #1
                           pAlignedArray2,              // Dataset #2
                           "Aligned Signal #2",         // Dataset #2 title
                           "lines",                     // Graph type #2
                           "orange",                    // Colour #2
                           "Magnitude",                 // Y-Axis label #2
                           GPC_AUTO_SCALE,              // Scaling mode #2
                           GPC_SIGNED,                  // Sign mode #2
                           outputLength);               // Dataset lengths

    printf ("Hit <Carriage Return> to continue ...\n"); getchar ();

    gpc_close (h2DPlot);

    free (pDataArray1);                                 // Free memory
    free (pDataArray2);
    free (pAlignedArray1);
    free (pAlignedArray2);
}

