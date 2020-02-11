// SigLib - This example generates a delay in ms of the amount defined on the command line
// Copyright (C) 2020 Sigma Numerix Ltd.

// Include files
#include <stdio.h>
#include <siglib.h>                                 // SigLib DSP library

int main( int argc, char **argv )

{
    int RequiredDelay;

    if (argc != 2) {
        printf ("Usage   : msDelay delay\n");
        printf ("Example : msDelay 2000\n\n");
        RequiredDelay = 2000;
    }
    else {
        RequiredDelay = (int)atol(argv[1]);
    }

    printf ("Waiting for delay = %d (ms)\n\n", (int)RequiredDelay);
    SUF_MSDelay ((SLFixData_t)RequiredDelay);
    printf ("Finished\n");

    return (0);
}

