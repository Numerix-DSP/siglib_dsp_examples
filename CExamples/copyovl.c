// SigLib Copy With Overlap Example

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include <nhl.h>

// Define constants
#define SRC_ARRAY_LENGTH    10                      // Source array lengths
#define OVERLAP_LENGTH      3                       // Three sample overlap between arrays
#define DST_ARRAY_LENGTH    5                       // Destination array length

// Declare global variables and arrays
static const SLData_t   SrcArray0 [] = {0., 1., 2., 3., 4., 5., 6., 7., 8., 9.};
static const SLData_t   SrcArray1 [] = {10., 11., 12., 13., 14., 15., 16., 17., 18., 19.};
static const SLData_t   SrcArray2 [] = {20., 21., 22., 23., 24., 25., 26., 27., 28., 29.};
static const SLData_t   SrcArray3 [] = {30., 31., 32., 33., 34., 35., 36., 37., 38., 39.};
static const SLData_t   SrcArray4 [] = {40., 41., 42., 43., 44., 45., 46., 47., 48., 49.};

static const SLData_t   *ArrayPtrs[] =
{
    SrcArray0, SrcArray1, SrcArray2, SrcArray3, SrcArray4
};


static SLData_t         DstArray [SRC_ARRAY_LENGTH];    // Equal to source array lengths for final tests
static SLData_t         OverlapArray [OVERLAP_LENGTH];

void main (void)
{
    SLArrayIndex_t  i;
    SLArrayIndex_t  OverlapSrcArrayIndex;

    printf ("Hit <Carriage Return> to perform 60%% overlap between short sections of larger arrays ...");
    SIF_CopyWithOverlap (&OverlapSrcArrayIndex);    // Pointer to source array index

    for (i = 0; i < 4; i++) {
        SDA_CopyWithOverlap (SrcArray0,             // Pointer to source array
                             DstArray,              // Pointer to destination array
                             OverlapArray,          // Pointer to overlap array
                             &OverlapSrcArrayIndex, // Pointer to source array index
                             SRC_ARRAY_LENGTH,      // Source array length
                             OVERLAP_LENGTH,        // Overlap length
                             DST_ARRAY_LENGTH);     // Destination array length

        printf ("OverlapSrcArrayIndex = %d\n", OverlapSrcArrayIndex);

        if (OverlapSrcArrayIndex >= SRC_ARRAY_LENGTH) { // Src array overlap detected - ensure array filled
            SDA_CopyWithOverlap (SrcArray1,             // Pointer to source array
                                 DstArray,              // Pointer to destination array
                                 OverlapArray,          // Pointer to overlap array
                                 &OverlapSrcArrayIndex, // Pointer to source array index
                                 SRC_ARRAY_LENGTH,      // Source array length
                                 OVERLAP_LENGTH,        // Overlap length
                                 DST_ARRAY_LENGTH);     // Destination array length
            printf ("OverlapSrcArrayIndex = %d\n", OverlapSrcArrayIndex);
        }
        print_buffer (DstArray, DST_ARRAY_LENGTH);
    }

    for (i = 0; i < 4; i++) {
        SDA_CopyWithOverlap (SrcArray1,             // Pointer to source array
                             DstArray,              // Pointer to destination array
                             OverlapArray,          // Pointer to overlap array
                             &OverlapSrcArrayIndex, // Pointer to source array index
                             SRC_ARRAY_LENGTH,      // Source array length
                             OVERLAP_LENGTH,        // Overlap length
                             DST_ARRAY_LENGTH);     // Destination array length
        printf ("OverlapSrcArrayIndex = %d\n", OverlapSrcArrayIndex);
        print_buffer (DstArray, DST_ARRAY_LENGTH);
    }

    printf ("Hit <Carriage Return> to perform 40%% overlap between arrays ...");
    getchar ();

            // Note : The following code shows how this function is traditionally used
                    // While there is data to process it,
                    // otherwise get some new data then process that

    SIF_CopyWithOverlap (&OverlapSrcArrayIndex);    // Pointer to source array index

    for (i = 0; i < 5; i++) {
        while (SDA_CopyWithOverlap (ArrayPtrs[i],   // Pointer to source array
                             DstArray,              // Pointer to destination array
                             OverlapArray,          // Pointer to overlap array
                             &OverlapSrcArrayIndex, // Pointer to source array index
                             SRC_ARRAY_LENGTH,      // Source array length
                             4,                     // Overlap length
                             SRC_ARRAY_LENGTH) <    // Destination array length
                                SRC_ARRAY_LENGTH) {

            printf ("OverlapSrcArrayIndex = %d\n", OverlapSrcArrayIndex);
            print_buffer (DstArray, SRC_ARRAY_LENGTH);
        }
    }
}

