#!/bin/bash

# Shell script for creating Java wrapper for accessing SigLib library from Android Java - tested under Cygwin
# Ensure swig.exe is in your system PATH
# Ensure the appropriate include paths are specified, for example :
#     set INCLUDE=C:\Users\John\AppData\Local\Android\Sdk\ndk-bundle\sysroot\usr\include;%INCLUDE%
#     set INCLUDE=C:\Users\John\AppData\Local\Android\Sdk\ndk-bundle\sysroot\usr\include\i686-linux-android\;%INCLUDE%

#!/bin/bash

# Shell script for building SigLib with SWIG wrapper for Android - tested under Cygwin

# Delete the wrapper c file so that if it is not regenerated then the compile will fail
rm -f siglib_wrap_wrap.c

cp -f typemaps.java.i typemaps.i
cp -f siglib_wrap.java.i siglib_wrap.i

# swig -java -I/cygdrive/c/siglib/include -includeall siglib_wrap.i
swig -java -I/cygdrive/c/siglib/include -includeall -package siglib_wrap siglib_wrap.i


ndk-build NDK_PROJECT_PATH=$(pwd) APP_BUILD_SCRIPT=$(pwd)/jni/Android.mk clean
ndk-build NDK_PROJECT_PATH=$(pwd) APP_BUILD_SCRIPT=$(pwd)/jni/Android.mk

cp obj/local/arm64-v8a/* ../lib/Android_SWIG/arm64-v8a
cp obj/local/armeabi-v7a/* ../lib/Android_SWIG/armeabi-v7a
cp obj/local/x86/* ../lib/Android_SWIG/x86
cp obj/local/x86_64/* ../lib/Android_SWIG/x86_64

# ndk-build NDK_PROJECT_PATH=$(pwd) APP_BUILD_SCRIPT=$(pwd)/jni/Android.mk clean
