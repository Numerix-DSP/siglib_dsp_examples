// .DAT to .WAV file conversion program
// Copyright (C) 2020 Sigma Numerix Ltd.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <siglib.h>                                 // SigLib DSP library

double  *p_DataArray;
#define SAMPLE_SIZE     128

SLWavFileInfo_s wavInfo;

void main( int argc, char **argv )
{
    int     SampleCount;
    FILE    *pInputFile, *pOutputFile;
    int     TotalSampleCount = 0;

    char    DatFileName[256];
    char    WavFileName[256];

    if (argc != 2) {
        printf("Useage : dat2wav filename (no extension)\n");
        return;
    }

    strcpy (DatFileName, argv[1]);
    strcat (DatFileName, ".dat");

    strcpy (WavFileName, argv[1]);
    strcat (WavFileName, ".wav");

    printf ("Dat filename = %s\n", DatFileName);
    printf ("Wav filename = %s\n", WavFileName);

    p_DataArray = (double *)malloc (SAMPLE_SIZE * sizeof (double));

    if ((pInputFile = fopen(DatFileName, "rb")) == NULL) {  // Note this file is binary
        printf ("Error opening input file %s\n", DatFileName);
        exit (1);
    }

    if ((pOutputFile = fopen(WavFileName, "wb")) == NULL) { // Note this file is binary
        printf ("Error opening output file %s\n", WavFileName);
        exit (1);
    }

    printf ("Input sample rate = %lf\n", SUF_DatReadHeader (pInputFile));

    wavInfo = SUF_WavSetInfo ((long)SUF_DatReadHeader (pInputFile), 0, 1, 16, 2, 1);

    SUF_WavWriteHeader (pOutputFile, wavInfo);            // Write dummy header to output file

    while ((SampleCount = SUF_DatReadData (p_DataArray, pInputFile, SAMPLE_SIZE)) != 0) { // Successively read arrays of upto 128 samples
    printf ("SampleCount = %d\n", SampleCount);
        TotalSampleCount += SampleCount;
        SUF_WavWriteData (p_DataArray, pOutputFile, wavInfo, SampleCount);
    }
                                                        // Write last block of data
    printf ("Total number of samples read = %d\n", TotalSampleCount);

    wavInfo.NumberOfSamples = TotalSampleCount;         // Set total data length
    rewind (pOutputFile);                               // Rewind pointer to start of file
    SUF_WavWriteHeader (pOutputFile, wavInfo);            // Overwrite the header information

    fclose (pInputFile);
    fclose (pOutputFile);

    free (p_DataArray);                                 // Free memory
}

