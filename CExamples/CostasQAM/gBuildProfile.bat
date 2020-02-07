@echo off

rem Batch file for compiling both the modulator and demodulator, with GCC
rem then profiling both programs.

del *.exe
del *.dot
del *.png
del *.wav
del *.out

echo Compiling pi/4 DQPSK modulator
call gb_profile dqpskmod
echo pi/4 DQPSK modulator has been compiled

call gb_profile dqpskdem
echo pi/4 DQPSK demodulator has been compiled

dqpskmod
move gmon.out dqpskmod.out

dqpskdem base
move gmon.out dqpskdem.out

echo Results written to DemodOutput.txt

rem Extract profile data and write to .dot file
rem Note, we are ignoring some functions which are not part of the real-time modem functionality

gprof dqpskmod.exe dqpskmod.out -Qinject_noise -QSIF_PiByFourDQpskModulate -QSDA_SignalGenerate -Qrand -QSIF_Fir | python %HOMEPATH%\python\gprof2dot\gprof2dot.py > dqpskmod.dot
gprof dqpskdem.exe dqpskdem.out | python %HOMEPATH%\python\gprof2dot\gprof2dot.py > dqpskdem.dot

dot -Tpng -odqpskmod.png dqpskmod.dot
dot -Tpng -odqpskdem.png dqpskdem.dot

echo Profile images written to .png files

del *.o

