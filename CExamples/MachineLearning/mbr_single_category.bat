@echo off

REM Build and run single category training and validation
REM Epoch count in validation only used for graph titles

call mbuild_single_category.bat

del "PreProcessed\*.*" /Q
del "weightCoefficientsFiles\*.*" /Q

preprocess_wav Machine1 0
preprocess_wav Machine2 1
type "PreProcessed\Training_Machine1_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine2_1.csv" >> Tdata.csv
type "PreProcessed\Validation_Machine1_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine2_1.csv" >> Vdata.csv
network_train_single_category -0 Machine1 -1 Machine2 -e 6
network_validate_single_category -0 Machine1 -1 Machine2 -e 6
echo.

preprocess_wav Machine1 0
preprocess_wav Machine3 1
type "PreProcessed\Training_Machine1_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine3_1.csv" >> Tdata.csv
type "PreProcessed\Validation_Machine1_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine3_1.csv" >> Vdata.csv
network_train_single_category -0 Machine1 -1 Machine3 -e 6
network_validate_single_category -0 Machine1 -1 Machine3 -e 6
echo.

preprocess_wav Machine1 0
preprocess_wav Machine4 1
type "PreProcessed\Training_Machine1_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine4_1.csv" >> Tdata.csv
type "PreProcessed\Validation_Machine1_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine4_1.csv" >> Vdata.csv
network_train_single_category -0 Machine1 -1 Machine4 -e 6
network_validate_single_category -0 Machine1 -1 Machine4 -e 6
echo.

preprocess_wav Machine2 0
preprocess_wav Machine3 1
type "PreProcessed\Training_Machine2_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine3_1.csv" >> Tdata.csv
type "PreProcessed\Validation_Machine2_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine3_1.csv" >> Vdata.csv
network_train_single_category -0 Machine2 -1 Machine3 -e 6
network_validate_single_category -0 Machine2  -1 Machine3 -e 6
echo.

preprocess_wav Machine2 0
preprocess_wav Machine4 1
type "PreProcessed\Training_Machine2_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine4_1.csv" >> Tdata.csv
type "PreProcessed\Validation_Machine2_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine4_1.csv" >> Vdata.csv
network_train_single_category -0 Machine2 -1 Machine4 -e 6
network_validate_single_category -0 Machine2  -1 Machine4 -e 6
echo.

preprocess_wav Machine3 0
preprocess_wav Machine4 1
type "PreProcessed\Training_Machine3_0.csv" > Tdata.csv
type "PreProcessed\Training_Machine4_1.csv" >> Tdata.csv
type "PreProcessed\Validation_Machine3_0.csv" > Vdata.csv
type "PreProcessed\Validation_Machine4_1.csv" >> Vdata.csv
network_train_single_category -0 Machine3 -1 Machine4 -e 6
network_validate_single_category -0 Machine3  -1 Machine4 -e 6
echo.
