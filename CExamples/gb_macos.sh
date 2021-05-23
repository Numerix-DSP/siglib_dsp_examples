#!/bin/bash

gcc -I ../../include -I ../../gnuplot_c/src $1.c -L ../../lib/macos -l siglib -L ../../gnuplot_c/src -l gnuplot_c -lm -o $1
