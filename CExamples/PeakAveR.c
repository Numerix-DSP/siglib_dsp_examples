// SigLib Peak To Average Ratio Example

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

// Define constants
#define ARRAY_LENGTH            10

// Declare global variables and arrays
static SLData_t     RealArray[ARRAY_LENGTH], ImagArray[ARRAY_LENGTH];

void main (void)

{
    SLArrayIndex_t  i;

    for (i = 0; i < ARRAY_LENGTH; i++) {
        RealArray[i] = (SLData_t)i;
        ImagArray[i] = (SLData_t)i;
    }

    printf ("The peak to average ratio = %lf\n",
            SDA_PeakToAverageRatio (RealArray,              // Pointer to source array
                                    ARRAY_LENGTH));         // Array length

    printf ("The peak to average power ratio (linear) = %lf\n",
            SDA_PeakToAveragePowerRatio (RealArray,             // Pointer to source array
                                         ARRAY_LENGTH));        // Array length

    printf ("The peak to average power ratio (dB) = %lf\n",
            SDA_PeakToAveragePowerRatioDB (RealArray,               // Pointer to source array
                                           ARRAY_LENGTH));          // Array length

    printf ("The complex peak to average ratio = %lf\n",
            SDA_PeakToAverageRatioComplex (RealArray,               // Pointer to source array
                                           ImagArray,               // Pointer to source array
                                           ARRAY_LENGTH));          // Array length

    printf ("The complex peak to average power ratio (linear) = %lf\n",
            SDA_PeakToAveragePowerRatioComplex (RealArray,              // Pointer to source array
                                                ImagArray,              // Pointer to source array
                                                ARRAY_LENGTH));         // Array length

    printf ("The complex peak to average power ratio (dB) = %lf\n",
            SDA_PeakToAveragePowerRatioComplexDB (RealArray,            // Pointer to source array
                                                  ImagArray,            // Pointer to source array
                                                  ARRAY_LENGTH));       // Array length

}

