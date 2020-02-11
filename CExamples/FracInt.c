// SigLib - Example program for fraction and integer fuctions
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

void main (void)

{

    printf ("int (10.4) = %lf\n", SDS_Int (10.4));
    printf ("int (10.5) = %lf\n", SDS_Int (10.5));
    printf ("int (10.6) = %lf\n", SDS_Int (10.6));
    printf ("int (-10.4) = %lf\n", SDS_Int (-10.4));
    printf ("int (-10.5) = %lf\n", SDS_Int (-10.5));
    printf ("int (-10.6) = %lf\n\n", SDS_Int (-10.6));

    printf ("frac (10.4) = %lf\n", SDS_Frac (10.4));
    printf ("frac (10.5) = %lf\n", SDS_Frac (10.5));
    printf ("frac (10.6) = %lf\n", SDS_Frac (10.6));
    printf ("frac (-10.4) = %lf\n", SDS_Frac (-10.4));
    printf ("frac (-10.5) = %lf\n", SDS_Frac (-10.5));
    printf ("frac (-10.6) = %lf\n\n", SDS_Frac (-10.6));

    printf ("absfrac (10.4) = %lf\n", SDS_AbsFrac (10.4));
    printf ("absfrac (10.5) = %lf\n", SDS_AbsFrac (10.5));
    printf ("absfrac (10.6) = %lf\n", SDS_AbsFrac (10.6));
    printf ("absfrac (-10.4) = %lf\n", SDS_AbsFrac (-10.4));
    printf ("absfrac (-10.5) = %lf\n", SDS_AbsFrac (-10.5));
    printf ("absfrac (-10.6) = %lf\n\n", SDS_AbsFrac (-10.6));

}

