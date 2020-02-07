// SigLib Comb Filter Example

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include "nhl.h"

// Define constants
#define SAMPLE_LENGTH       10
#define COMB_FILTER_LENGTH  2

// Declare global variables and arrays
static SLArrayIndex_t   CombFilterIndex;
static SLData_t         CombFilterSum;
static SLData_t         CombFilterStateArray [COMB_FILTER_LENGTH];
static const SLData_t   SourceArray [] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };
static SLData_t         DstArray [SAMPLE_LENGTH];

void    main(void);

void main(void)
{
    SLFixData_t i;

    SIF_Comb (CombFilterStateArray,                 // Pointer to filter delay state array
              &CombFilterIndex,                     // Pointer to filter index register
              &CombFilterSum,                       // Pointer to filter sum register
              COMB_FILTER_LENGTH);                  // Filter length

                                    // Apply comb filter and store filtered data
    for (i = 0; i < SAMPLE_LENGTH; i++) {
        DstArray[i] = SDS_Comb (SourceArray[i],         // Input data sample to be filtered
                                CombFilterStateArray,   // Pointer to filter state array
                                &CombFilterIndex,       // Pointer to filter index register
                                &CombFilterSum,         // Pointer to filter sum register
                                COMB_FILTER_LENGTH);    // Filter length
    }

    printf ("SDS_Comb results :\n");
    print_buffer (DstArray, SAMPLE_LENGTH);


    SIF_Comb (CombFilterStateArray,                 // Pointer to filter delay state array
              &CombFilterIndex,                     // Pointer to filter index register
              &CombFilterSum,                       // Pointer to filter sum register
              COMB_FILTER_LENGTH);                  // Filter length

                                    // Apply comb filter and store filtered data
    SDA_Comb (SourceArray,                          // Source array pointer
              DstArray,                             // Destination array pointer
              CombFilterStateArray,                 // Pointer to filter state array
              &CombFilterIndex,                     // Pointer to filter index register
              &CombFilterSum,                       // Pointer to filter sum register
              COMB_FILTER_LENGTH,                   // Filter length
              SAMPLE_LENGTH>>1);                    // Sample length

    printf ("SDA_Comb results [1-5]:\n");
    print_buffer (DstArray, SAMPLE_LENGTH>>1);

    SDA_Comb (SourceArray+5,                        // Source array pointer
              DstArray,                             // Destination array pointer
              CombFilterStateArray,                 // Pointer to filter state array
              &CombFilterIndex,                     // Pointer to filter index register
              &CombFilterSum,                       // Pointer to filter sum register
              COMB_FILTER_LENGTH,                   // Filter length
              SAMPLE_LENGTH>>1);                    // Sample length

    printf ("SDA_Comb results [6-10]:\n");
    print_buffer (DstArray, SAMPLE_LENGTH>>1);
}
