@echo off

rem Batch file for building and executing the helicopter examples with GCC
rem This build uses the static library, release model

del wavorder.exe
del *.gpdt
call gb wavorder
del *.obj
if exist wavorder.exe (
  wavorder %1
  if exist sc.gpdt gnuplot sgplot.gp
)
del *.gpdt
