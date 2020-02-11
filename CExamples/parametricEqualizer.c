// SigLib Parametric Equalizer calculation functions
// Copyright (C) 2020 Sigma Numerix Ltd.

#include <stdio.h>
#include <math.h>
#include <siglib.h>                                 // SigLib DSP library

int main (void)
{
    SLData_t Fl = 100.; SLData_t Fh = 200.;
    printf ("Fl = %lf, Fh = %lf, B.W. (Octaves) = %lf\n", Fl, Fh, SUF_FrequenciesToOctaves (Fl, Fh));

    printf ("Fl = %lf, Fh = %lf, Centre Frequency (Hz) = %lf\n", Fl, Fh, SUF_FrequenciesToCentreFreqHz (Fl, Fh));

    printf ("Fl = %lf, Fh = %lf, Q Factor = %lf\n", Fl, Fh, SUF_FrequenciesToQFactor (Fl, Fh));

    SLData_t BW = 2.;
    printf ("BW = %lf, Q Factor = %lf\n", BW, SUF_BandwidthToQFactor (BW));

    SLData_t QFactor = 2.;
    printf ("Q Factor = %lf, BW = %lf\n", QFactor, SUF_QFactorToBandwidth (QFactor));


    return 1;
}

