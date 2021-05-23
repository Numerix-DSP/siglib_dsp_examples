// SigLib example for calculating the speed of sound with change in temperature
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

#include <stdio.h>
#include <math.h>

void main (void)

{
    SLData_t temp = 20.;
    printf ("Speed Of Sound In Air at %.2lf \370C = %.2lf\n", temp, SDS_TemperatureToSpeedOfSoundInAir(temp));
}

