@echo off

rem Batch file for building and executing the helicopter examples with GCC
rem This build uses the static library, release model

del wavwfall.exe
del *.gpdt
call gb wavwfall
del *.obj
if exist wavwfall.exe (
  wavwfall %1
  if exist sc.gpdt gnuplot sgplot.gp
)
del *.gpdt

