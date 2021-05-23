#!/usr/bin/env bash

# Build and run single category training, with data augmentation and validation on motor mode recordings

./gbr_single_category.sh -0 MotorMode1 -1 MotorMode2 -c -e 20 -a 10 -l 128 -H 25
rm -f "PreProcessed\*.*"
rm -f "weightCoefficientsFiles\*.*"

# Add data augmentation
./preprocess_wav MotorMode1 0 10
./preprocess_wav MotorMode2 1 10

./gbr_single_category.sh -0 MotorMode1 -1 MotorMode2 -c -e 20 -a 10 -l 128 -H 25
echo " "



# # Add data augmentation
# ./preprocess_wav MotorMode1 0 10
# ./preprocess_wav MotorMode2 1 10

# type "PreProcessed\Training_MotorMode1_0.csv" > Tdata.csv
# type "PreProcessed\Training_MotorMode2_1.csv" >> Tdata.csv
# type "PreProcessed\Validation_MotorMode1_0.csv" > Vdata.csv
# type "PreProcessed\Validation_MotorMode2_1.csv" >> Vdata.csv

# network_train_single_category -0 MotorMode1 -1 MotorMode2 -e 20

# # Epoch count in validation only used for graph titles
# network_validate_single_category -0 MotorMode1 -1 MotorMode2 -e 20 -s
