#!/bin/bash

gcc -I ../../include -I ../../gnuplot_c/src $1.c -L ../../lib/linux -l siglib -L ../../gnuplot_c/src -l gnuplot_c -o $1 -lm
