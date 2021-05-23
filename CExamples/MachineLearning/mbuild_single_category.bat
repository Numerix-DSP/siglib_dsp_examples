@echo off

rem Batch file for building and executing application with Microsoft Visual Studio compiler
rem This build uses the static SigLib DSP library release

del *.exe

cl preprocess_wav.c -O2 -W4 -D "NETWORK_INPUT_SAMPLE_LENGTH=128" -D "SIGLIB_STATIC_LIB=1" -D "_CRT_SECURE_NO_WARNINGS=1" siglib.lib gnuplot_c.lib
cl network_train_single_category.c -O2 -W4 -D "NETWORK_INPUT_SAMPLE_LENGTH=128" -D "ACTIVATION_TYPE=SIGLIB_ACTIVATION_TYPE_LOGISTIC" -D "SIGLIB_STATIC_LIB=1" -D "_CRT_SECURE_NO_WARNINGS=1" siglib.lib gnuplot_c.lib
cl network_validate_single_category.c -O2 -W4 -D "NETWORK_INPUT_SAMPLE_LENGTH=128" -D "ACTIVATION_TYPE=SIGLIB_ACTIVATION_TYPE_LOGISTIC" -D "SIGLIB_STATIC_LIB=1" -D "_CRT_SECURE_NO_WARNINGS=1" siglib.lib gnuplot_c.lib
del *.obj

