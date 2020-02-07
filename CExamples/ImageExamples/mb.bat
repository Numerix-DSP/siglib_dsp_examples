@echo off

rem Batch file for building client application with Microsoft V9 compiler

cl %1.c -D "SIGLIB_STATIC_LIB=1" C:\siglib\lib\Microsoft\static_library\Release\siglib.lib gnuplot_c.lib
rem C:\siglib\nhl\nhl.lib
del *.obj

