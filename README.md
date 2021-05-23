# siglib_dsp_examples
SigLib DSP Library Example Programs

All of these example can be compiled using the free version of the SigLib DSP libary, available here : https://numerix-dsp.com/eval/.

Full library source available here : https://numerix-dsp.com/siglib/.

In order to use the library most efficiently, please ensure that you have configured your OS Environment Variables correctly.

Under Windows :

rem SigLib Environment Variables
set SIGLIB_PATH=C:\siglib                                           # Change this to match your SigLib root path
set PATH=%PATH%;%SIGLIB_PATH%\Utils;%SIGLIB_PATH%\Examples\FileIO

rem Microsoft Visual C/C++ SigLib Environment Variables
set INCLUDE=%INCLUDE%;%SIGLIB_PATH%\include;%SIGLIB_PATH%\nhl;%SIGLIB_PATH%\ngl;%SIGLIB_PATH%\gnuplot_c\src
set LIB=%LIB%;%SIGLIB_PATH%\lib\Microsoft\static_library_64\Release;%SIGLIB_PATH%\nhl;%SIGLIB_PATH%\ngl;%SIGLIB_PATH%\gnuplot_c\src

Under Linux :

# SigLib Environment Variables
export SIGLIB_PATH=/User/USERNAME/siglib                            # Change this to match your SigLib root path
export PATH=$PATH:$SIGLIB_PATH/Utils:$SIGLIB_PATH/Examples/FileIO

# GCC SigLib Environment Variables
export C_INCLUDE_PATH=$SIGLIB_PATH/include:$SIGLIB_PATH/nhl:$SIGLIB_PATH/gnuplot_c/src
export LIBRARY_PATH=$SIGLIB_PATH/lib/linux_64:$SIGLIB_PATH/nhl:$SIGLIB_PATH/gnuplot_c/src



(c) 2020 Sigma Numerix Ltd.
