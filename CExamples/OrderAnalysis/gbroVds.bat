@echo off

rem Batch file for building and running order analysis - variable speed - dB against speed with GCC

call gb order
order 10330.0 D S Variable.dat
