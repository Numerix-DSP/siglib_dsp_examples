@echo off

rem Batch file for building and running the application with GCC

del *.exe
del *.dat

gcc toneLevels.c -Wall -Wno-unused-value -std=gnu99 -l siglib -l gnuplot_c -o toneLevels.exe
if not exist toneLevels.exe (
    goto:EOF
)

REM echo Converting wav file to dat to display linear signal
REM wav2dat SineWave-5-25dB

echo Calculating Tone Levels
toneLevels SineWave-5-25dB

REM echo Plotting Tone Levels
REM gnuplot plotTone.gp

