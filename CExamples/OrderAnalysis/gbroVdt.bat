@echo off

rem Batch file for building and running order analysis - Variable speed - dB against time with GCC

call gb order
order 10330.0 D T Variable.dat
