#!/bin/bash

gcc -I ../../include -I ../../gnuplot_c/src -I ../../nhl $1.c -L ../../lib/linux_64 -l siglib -L ../../gnuplot_c/src -l gnuplot_c -L../../nhl -l nhl -lm -o $1
