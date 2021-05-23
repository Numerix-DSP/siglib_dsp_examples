#!/usr/bin/env bash

NETWORK_INPUT_SAMPLE_LENGTH=128
EPOCH_COUNT=5
NETWORK_HIDDEN_LAYER_NODES=40

rm -f "PreProcessed/*csv"

# Compare all modes
echo  Model Accuracy With Data Augmentation: Configuration: -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""

# Pre-process and Compile sources on first comparison
./gbr_multi_category.sh -c -p
echo ""

# Pre-process the data
./preprocess_wav Machine5_Medium 0 10
./preprocess_wav Machine5_Fast   1 10
./preprocess_wav Machine4_Mode1 2 10
./preprocess_wav Machine4_Mode2 3 10

cat "PreProcessed/Training_Machine5_Medium_0.csv" > Tdata.csv
cat "PreProcessed/Training_Machine5_Fast_1.csv" >> Tdata.csv
cat "PreProcessed/Training_Machine4_Mode1_2.csv" >> Tdata.csv
cat "PreProcessed/Training_Machine4_Mode2_3.csv" >> Tdata.csv

cat "PreProcessed/Validation_Machine5_Medium_0.csv" > Vdata.csv
cat "PreProcessed/Validation_Machine5_Fast_1.csv" >> Vdata.csv
cat "PreProcessed/Validation_Machine4_Mode1_2.csv" >> Vdata.csv
cat "PreProcessed/Validation_Machine4_Mode2_3.csv" >> Vdata.csv

./network_train_multi_category -0 Machine5_Medium -1 Machine5_Fast -2 Machine4_Mode1 -3 Machine4_Mode2 -e 5 -d
# Epoch count in validation only used for graph titles
./network_validate_multi_category -0 Machine5_Medium -1 Machine5_Fast -2 Machine4_Mode1 -3 Machine4_Mode2 -e 5 -d
