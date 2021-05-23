#!/usr/bin/env bash

# Shell script for building application with GCC

gcc $1.c -Wall -Wno-main -Wno-unused-value -std=c99 -l siglib -l gnuplot_c -o $1 -lm
