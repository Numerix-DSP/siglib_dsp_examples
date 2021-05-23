// SigLib Rotate Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

// Define constants
#define SAMPLE_LENGTH   8
#define AMPLITUDE       (SAMPLE_LENGTH >> 1)
#define OFFSET          (SAMPLE_LENGTH >> 1)

// Declare global variables and arrays
static SLData_t         *pData1, *pData2;
static SLData_t         RampPhase;
static SLArrayIndex_t   Rotation;

void main(void)
{
                                                    // Allocate memory
    pData1 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    pData2 = SUF_VectorArrayAllocate (SAMPLE_LENGTH);

    RampPhase = SIGLIB_ZERO;
    SDA_SignalGenerateRamp  (pData1,                // Pointer to destination array
                             AMPLITUDE,             // Amplitude
                             OFFSET,                // D.C. Offset
                             &RampPhase,            // Phase - maintained across array boundaries
                             SAMPLE_LENGTH);        // Array length

    printf ("Input Array\n");
    SUF_PrintArray (pData1, SAMPLE_LENGTH);

    Rotation = 1;
    SDA_Rotate (pData1,                             // Pointer to source array
                pData2,                             // Pointer to destination array
                Rotation,                           // Rotation length - samples
                SAMPLE_LENGTH);                     // Source array length
    printf ("\nRotation = %d\n", Rotation);
    SUF_PrintArray (pData2, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    Rotation = 2;
    SDA_Rotate (pData1,                             // Pointer to source array
                pData2,                             // Pointer to destination array
                Rotation,                           // Rotation length - samples
                SAMPLE_LENGTH);                     // Source array length
    printf ("\nRotation = %d\n", Rotation);
    SUF_PrintArray (pData2, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    Rotation = 3;
    SDA_Rotate (pData1, pData2, Rotation, SAMPLE_LENGTH);
    printf ("\nRotation = %d\n", Rotation);
    SDA_Rotate (pData1,                             // Pointer to source array
                pData2,                             // Pointer to destination array
                Rotation,                           // Rotation length - samples
                SAMPLE_LENGTH);                     // Source array length
    printf ("Please hit <Return> to continue\n"); getchar ();

    Rotation = 4;
    SDA_Rotate (pData1,                             // Pointer to source array
                pData2,                             // Pointer to destination array
                Rotation,                           // Rotation length - samples
                SAMPLE_LENGTH);                     // Source array length
    printf ("\nRotation = %d\n", Rotation);
    SUF_PrintArray (pData2, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    Rotation = 5;
    SDA_Rotate (pData1,                             // Pointer to source array
                pData2,                             // Pointer to destination array
                Rotation,                           // Rotation length - samples
                SAMPLE_LENGTH);                     // Source array length
    printf ("\nRotation = %d\n", Rotation);
    SUF_PrintArray (pData2, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    Rotation = 6;
    SDA_Rotate (pData1,                             // Pointer to source array
                pData2,                             // Pointer to destination array
                Rotation,                           // Rotation length - samples
                SAMPLE_LENGTH);                     // Source array length
    printf ("\nRotation = %d\n", Rotation);
    SUF_PrintArray (pData2, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    Rotation = 7;
    SDA_Rotate (pData1,                             // Pointer to source array
                pData2,                             // Pointer to destination array
                Rotation,                           // Rotation length - samples
                SAMPLE_LENGTH);                     // Source array length
    printf ("\nRotation = %d\n", Rotation);
    SUF_PrintArray (pData2, SAMPLE_LENGTH);

    SUF_MemoryFree (pData1);                        // Free memory
    SUF_MemoryFree (pData2);

}

