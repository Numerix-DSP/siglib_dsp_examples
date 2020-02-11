// SigLib Select Minimum And Maximum Values In Two Arrays
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include "nhl.h"

// Define constants
#define SAMPLE_LENGTH       8

// Declare global variables and arrays
static const SLData_t   Data1Real[] = { 0.0,  1.0,  2.0,  3.0, 4.0,  5.0,  6.0,  7.0};
static const SLData_t   Data1Imag[] = {-0.0, -1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0};

static const SLData_t   Data2Real[] = { 7.0,  6.0,  5.0,  4.0, 3.0,  2.0,  1.0,  0.0};
static const SLData_t   Data2Imag[] = {-7.0, -6.0, -5.0, -4.0, -3.0, -2.0, -1.0, -0.0};

static SLData_t     *ResultReal, *ResultImag;

void main(void)
{
                                                    // Allocate memory
    ResultReal = SUF_VectorArrayAllocate (SAMPLE_LENGTH);
    ResultImag = SUF_VectorArrayAllocate (SAMPLE_LENGTH);

    printf ("Input Array\n");
    print_buffer (Data1Real, SAMPLE_LENGTH);

    SDA_SelectMax (Data1Real,                       // Pointer to source array 1
                   Data2Real,                       // Pointer to source array 2
                   ResultReal,                      // Pointer to destination array
                   SAMPLE_LENGTH);                  // Array length
    printf ("\nMaximum\n");
    print_buffer (ResultReal, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    SDA_SelectMin (Data1Real,                       // Pointer to source array 1
                   Data2Real,                       // Pointer to source array 2
                   ResultReal,                      // Pointer to destination array
                   SAMPLE_LENGTH);                  // Array length
    printf ("\nMinimum\n");
    print_buffer (ResultReal, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    SDA_SelectMagnitudeSquaredMax (Data1Real,       // Pointer to real source array 1
                                   Data1Imag,       // Pointer to imaginary source array 1
                                   Data2Real,       // Pointer to real source array 2
                                   Data2Imag,       // Pointer to imaginary source array 2
                                   ResultReal,      // Pointer to real destination array
                                   ResultImag,      // Pointer to imaginary destination array
                                   SAMPLE_LENGTH);  // Array length
    printf ("\nMaximum (Real)\n");
    print_buffer (ResultReal, SAMPLE_LENGTH);
    printf ("\nMaximum (Imaginary)\n");
    print_buffer (ResultImag, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    SDA_SelectMagnitudeSquaredMax (Data1Real,       // Pointer to real source array 1
                                   Data1Imag,       // Pointer to imaginary source array 1
                                   Data2Real,       // Pointer to real source array 2
                                   Data2Imag,       // Pointer to imaginary source array 2
                                   ResultReal,      // Pointer to real destination array
                                   ResultImag,      // Pointer to imaginary destination array
                                   SAMPLE_LENGTH);  // Array length
    printf ("\nMinimum (Real)\n");
    print_buffer (ResultReal, SAMPLE_LENGTH);
    printf ("\nMinimum (Imaginary)\n");
    print_buffer (ResultImag, SAMPLE_LENGTH);
    printf ("Please hit <Return> to continue\n"); getchar ();

    SUF_MemoryFree (ResultReal);                    // Free memory
    SUF_MemoryFree (ResultImag);

}


