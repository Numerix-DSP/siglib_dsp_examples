@echo off

rem Batch file for building and running order analysis with linear scaling - Const.dat with GCC

call gb order
order 10162.0 l T Const.dat
