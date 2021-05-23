@echo off

REM Build and run multi category data augmentation, training and validation + plot the confusion matrix

del "PreProcessed\*.*" /Q
del "weightCoefficientsFiles\*.*" /Q

rem Pre-process the data
preprocess_wav Machine5_Medium 0 10
preprocess_wav Machine5_Fast   1 10
preprocess_wav Machine4_Mode1 2 10
preprocess_wav Machine4_Mode2 3 10


type "PreProcessed\Training_Machine5_Medium_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine5_Fast_1.csv" >> Tdata.csv
type "PreProcessed\Training_Machine4_Mode1_2.csv" >> Tdata.csv
type "PreProcessed\Training_Machine4_Mode2_3.csv" >> Tdata.csv

type "PreProcessed\Validation_Machine5_Medium_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine5_Fast_1.csv" >> Vdata.csv
type "PreProcessed\Validation_Machine4_Mode1_2.csv" >> Vdata.csv
type "PreProcessed\Validation_Machine4_Mode2_3.csv" >> Vdata.csv


network_train_multi_category -0 Machine5_Medium -1 Machine5_Fast -2 Machine4_Mode1 -3 Machine4_Mode2 -e 5 -d
IF %ERRORLEVEL% LSS 0 GOTO End

REM Epoch count in validation only used for graph titles
network_validate_multi_category -0 Machine5_Medium -1 Machine5_Fast -2 Machine4_Mode1 -3 Machine4_Mode2 -e 5 -d -m

:End
