@echo off

rem Batch file for building and running order analysis - Variable speed - dB against time

call mb_64 order
order 10330.0 D T Variable.dat
