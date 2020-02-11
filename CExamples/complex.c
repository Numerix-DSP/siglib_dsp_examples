// SigLib Complex Number Example
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library
#include "nhl.h"

void main (void)
{
    SLComplexRect_s     C1, C2, C3;
    SLComplexPolar_s    P1;

    SLComplexRect_s     Numerator = SCV_Rectangular (2.0, 6.0);
    SLComplexRect_s     Denominator = SCV_Rectangular (4.0, 1.0);
    SLComplexRect_s     Result;
    SLData_t            ResultReal, ResultImag;

    printf ("\nPolar vector\n");
                            // Create a polar number from magnitude and phase
    P1 = SCV_Polar (0.9, (30. * SIGLIB_TWO_PI) / 360.);
    print_polar (P1);

    printf ("\nPolar vector converted to complex\n");
    C1 = SCV_PolarToRectangular (P1);
    print_rectangular (C1);

    printf ("\nComplex vector\n");
                            // Create a rectangular number from real and imaginary components
    C2 = SCV_Rectangular (3.4, -5.6);
    print_rectangular (C2);

    printf ("\nComplex vector equate\n");
    C2 = C1;
    print_rectangular (C2);


    printf ("\nComplex vector inversion types 1 and 2\n");
                            // Create a rectangular number from real and imaginary components
    C2 = SCV_Rectangular (3.4, -5.6);
    C3 = SCV_Inverse (C1);
    print_rectangular (C3);


    printf ("\nComplex vector division types 1 and 2\n");
    C3 = SCV_Divide (C1, C2);
    print_rectangular (C3);


    printf ("\nComplex multiply\n");
    Result = SCV_Multiply (Numerator, Denominator);
    printf ("(2 + j6) * (4 + j1) = %lf +j%lf\n", Result.real, Result.imag);
    SDS_ComplexMultiply (2.0, 6.0, 4.0, 1.0, &ResultReal, &ResultImag);
    printf ("(2 + j6) * (4 + j1) = %lf +j%lf\n\n", ResultReal, ResultImag);

    printf ("\nComplex inverse\n");
    Result = SCV_Inverse (Denominator);
    printf ("1 / (4 + j1) = %lf +j%lf\n", Result.real, Result.imag);
    SDS_ComplexInverse (4.0, 1.0, &ResultReal, &ResultImag);
    printf ("1 / (4 + j1) = %lf +j%lf\n\n", ResultReal, ResultImag);

    Result = SCV_Multiply (Numerator, SCV_Inverse (Denominator));
    printf ("(2 + j6) / (4 + j1) = %lf +j%lf\n", Result.real, Result.imag);

    printf ("\nComplex divide\n");
    Result = SCV_Divide (Numerator, Denominator);
    printf ("(2 + j6) / (4 + j1) = %lf +j%lf\n", Result.real, Result.imag);
    SDS_ComplexDivide (2.0, 6.0, 4.0, 1.0, &ResultReal, &ResultImag);
    printf ("(2 + j6) / (4 + j1) = %lf +j%lf\n", ResultReal, ResultImag);


}


