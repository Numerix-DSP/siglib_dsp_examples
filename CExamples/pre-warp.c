// SigLib Bilinear Transform Pre-warping Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

void main (void)
{

    printf ("Desired frequency = %lf, Sample rate = %lf, Warped frequency = %lf\n", 5.0, 25.0, SDS_PreWarp (5.0, 25.0));
    printf ("Desired frequency = %lf, Sample rate = %lf, Warped frequency = %lf\n", 0.4, 2.0, SDS_PreWarp (0.4, 2.0));

}

