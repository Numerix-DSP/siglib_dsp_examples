// SigLib Mathematics Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

void main (void)

{
    printf ("SDS_Factorial (%lf) = %lf\n", 0.0, SDS_Factorial (0.0));
    printf ("SDS_Factorial (%lf) = %lf\n", 1.0, SDS_Factorial (1.0));
    printf ("SDS_Factorial (%lf) = %lf\n", 3.0, SDS_Factorial (3.0));
    printf ("SDS_Factorial (%lf) = %lf\n", 5.0, SDS_Factorial (5.0));

    printf ("SDS_Permutations (%lf, %lf) = %lf\n", 5.0, 3.0, SDS_Permutations (5.0,     // n
                                                                               3.0));   // k
    printf ("SDS_Combinations (%lf, %lf) = %lf\n", 5.0, 3.0, SDS_Combinations (5.0,     // n
                                                                               3.0));   // k

}
