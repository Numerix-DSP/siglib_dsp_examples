// SigLib - Trigger Selector Example

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

// Define constants
#define SAMPLE_LENGTH           10

// Declare global variables and arrays
                                        // Test trigger input sequence
static const SLData_t   Trigger [] = { 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
static const SLData_t   Src [] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };

static SLData_t Dst [SAMPLE_LENGTH];


void main (void)

{
    SLArrayIndex_t      i;
    SLArrayIndex_t      OutputCount;

    OutputCount = SDA_TriggerSelector (Src,             // Source data sequence
                                       Dst,             // Destination data sequence
                                       Trigger,         // Trigger sequence
                                       SAMPLE_LENGTH);  // Length of source sequence

    SUF_ClearDebugfprintf ();
    SUF_Debugfprintf ("Trigger output :\n");

    for (i = 0; i < OutputCount; i++) {
        printf ("%1.1lf\n", Dst[i]);
    }
}


