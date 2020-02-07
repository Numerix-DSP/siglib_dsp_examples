@echo off

rem Batch file for building and executing client application with Microsoft Visual Studio compiler
rem This build uses the static library, release model

del %1.exe

cl %1.c -W4 -D "SIGLIB_STATIC_LIB=1" -D "_CRT_SECURE_NO_WARNINGS=1" siglib.lib gnuplot_c.lib nhl.lib
del *.obj
if exist %1.exe %1 %2 %3 %4 %5 %6 %7 %8 %9
