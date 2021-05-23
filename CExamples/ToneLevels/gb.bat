@echo off

rem Batch file for building and executing client application with GCC

gcc %1.c -Wall -Wno-main -Wno-unused-value -std=gnu99 -l siglib -l gnuplot_c -o %1.exe
REM del *.o
