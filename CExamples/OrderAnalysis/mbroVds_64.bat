@echo off

rem Batch file for building and running order analysis - variable speed - dB against speed

call mb_64 order
order 10330.0 D S Variable.dat
