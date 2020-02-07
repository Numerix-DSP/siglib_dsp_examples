@echo off

rem Batch file for building and executing client application with GCC

del alignwavs.exe

call gb alignwavs
if exist alignwavs.exe alignwavs.exe Kipling_If_Original_16kHz.wav Kipling_If_PlusDelayAndNoise_16kHz.wav
