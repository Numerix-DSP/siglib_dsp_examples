@echo off

REM Build and run single category training, with data augmentation and validation on motor mode recordings

call mbuild_single_category.bat

del "PreProcessed\*.*" /Q
del "weightCoefficientsFiles\*.*" /Q

REM Add data augmentation
preprocess_wav MotorMode1 0 10
preprocess_wav MotorMode2 1 10

type "PreProcessed\Training_MotorMode1_0.csv" > Tdata.csv
type "PreProcessed\Training_MotorMode2_1.csv" >> Tdata.csv
type "PreProcessed\Validation_MotorMode1_0.csv" > Vdata.csv
type "PreProcessed\Validation_MotorMode2_1.csv" >> Vdata.csv

network_train_single_category -0 MotorMode1 -1 MotorMode2 -e 20

REM Epoch count in validation only used for graph titles
network_validate_single_category -0 MotorMode1 -1 MotorMode2 -e 20 -s
echo.

