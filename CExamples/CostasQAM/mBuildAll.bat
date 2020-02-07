@echo off

rem Batch file for compiling both the modulator and demodulator

echo Compiling pi/4 DQPSK modulator
call mb dqpskmod

echo Compiling pi/4 DQPSK demodulator
call mb dqpskdem

