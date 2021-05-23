#!/usr/bin/env bash

# Shell script for building application with GCC

gcc $1.c analog_io.c -DRASPBERRY_PI=1 -Wall -Wno-main -Wno-unused-value -std=c99 -l siglib -l gnuplot_c -l portaudio -o $1 -lm
