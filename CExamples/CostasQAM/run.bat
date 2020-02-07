@echo off

rem Batch file for executing both the modulator and demodulator

dqpskmod

echo Hit CR to execute demodulator
pause
dqpskdem base

echo Results written to DemodOutput.txt
