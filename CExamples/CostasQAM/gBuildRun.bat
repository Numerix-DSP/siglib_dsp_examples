@echo off

rem Batch file for compiling both the modulator and demodulator, with GCC
rem then executing both programs.

del *.exe
del base.wav
del DemodOutput.txt

echo Compiling pi/4 DQPSK modulator
call gb dqpskmod
echo pi/4 DQPSK modulator has been compiled

REM echo Hit <CR> to compile pi/4 DQPSK demodulator
REM pause

call gb dqpskdem
echo pi/4 DQPSK demodulator has been compiled

REM echo Hit <CR> to execute modulator
REM pause

dqpskmod

REM echo Hit <CR> to execute demodulator
REM pause

dqpskdem base

echo Results written to DemodOutput.txt :

type DemodOutput.txt
