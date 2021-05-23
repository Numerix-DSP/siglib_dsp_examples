@echo off

REM Build and run multi category training and validation + plot the confusion matrix

call mbuild_multi_category.bat

del "PreProcessed\*.*" /Q
del "weightCoefficientsFiles\*.*" /Q

rem Pre-process the data

preprocess_wav Machine1 0
preprocess_wav Machine2 1
preprocess_wav Machine3 2
preprocess_wav Machine4 3


type "PreProcessed\Training_Machine1_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine2_1.csv" >> Tdata.csv
type "PreProcessed\Training_Machine3_2.csv" >> Tdata.csv
type "PreProcessed\Training_Machine4_3.csv" >> Tdata.csv

type "PreProcessed\Validation_Machine1_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine2_1.csv" >> Vdata.csv
type "PreProcessed\Validation_Machine3_2.csv" >> Vdata.csv
type "PreProcessed\Validation_Machine4_3.csv" >> Vdata.csv

network_train_multi_category -0 Machine1 -1 Machine2 -2 Machine3 -3 Machine4 -e 6 -d
IF %ERRORLEVEL% LSS 0 GOTO End

REM Epoch count in validation only used for graph titles
network_validate_multi_category -0 Machine1 -1 Machine2 -2 Machine3 -3 Machine4 -e 6 -m

:End
