@echo off

rem Batch file for building client application with Microsoft (V15.0 / Visual Studio 9) compiler

cl %1.c -W4 -D "SIGLIB_STATIC_LIB=1" -D "_CRT_SECURE_NO_WARNINGS=1" C:\siglib\lib\Microsoft\static_library\Release\siglib.lib gnuplot_c.lib C:\siglib\nhl\nhl.lib
del *.obj

