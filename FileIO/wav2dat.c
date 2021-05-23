// .WAV file function test program
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
    int     sampleIndex = 0l;

    char    WavFileName[256];
    char    DatFileName[256];

    if (argc != 2) {
        printf("Useage : wav2dat filename (no extension)\n");
        return;
    }

    strcpy (WavFileName, argv[1]);
    strcat (WavFileName, ".wav");

    strcpy (DatFileName, argv[1]);
    strcat (DatFileName, ".dat");

    printf ("Wav filename = %s\n", WavFileName);
    printf ("Dat filename = %s\n", DatFileName);

    p_DataArray = (double *)malloc (SAMPLE_SIZE * sizeof (double));


    if ((pInputFile = fopen(WavFileName, "rb")) == NULL) {  // Note this file is binary
        printf ("Error opening input file %s\n", WavFileName);
        exit (1);
    }

    if ((pOutputFile = fopen(DatFileName, "wb")) == NULL) { // Note this file is binary
        printf ("Error opening output file %s\n", DatFileName);
        fclose (pInputFile);
        exit (1);
    }

    wavInfo = SUF_WavReadHeader (pInputFile);
    SUF_WavDisplayInfo (wavInfo);
    if (wavInfo.NumberOfChannels != 1) {                // Check how many channels
        printf ("Number of channels in %s = %d\n", WavFileName, wavInfo.NumberOfChannels);
        printf ("This app requires a mono .wav file\n");
        exit (1);
    }

    SUF_DatWriteHeader (pOutputFile, wavInfo.SampleRate);    // Write header - must be done ahead of writing data

    while ((SampleCount = SUF_WavReadData (p_DataArray, pInputFile, wavInfo, SAMPLE_SIZE)) != 0) {  // Successively read arrays of upto 128 samples
        TotalSampleCount += SampleCount;
        sampleIndex += SUF_DatWriteData (p_DataArray, pOutputFile, wavInfo.SampleRate, sampleIndex, SampleCount);
    }
                                                        // Write last block of data
    printf ("Total number of samples read = %d\n", TotalSampleCount);

    fclose (pInputFile);
    fclose (pOutputFile);

    free (p_DataArray);                                 // Free memory
}

