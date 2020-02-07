@echo off

rem Batch file for building and running order analysis with linear scaling - Const.dat

call mb_64 order
order 10162.0 l T Const.dat
