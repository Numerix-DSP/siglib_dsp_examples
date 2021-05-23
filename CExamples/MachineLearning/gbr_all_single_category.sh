#!/usr/bin/env bash

NETWORK_INPUT_SAMPLE_LENGTH=128
EPOCH_COUNT=6
NETWORK_HIDDEN_LAYER_NODES=25

rm -f "PreProcessed/*csv"

# Compare all modes
echo  Model Accuracy: Configuration: -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""

# Pre-process and Compile sources on first comparison
./gbr_single_category.sh -c -p -0 Machine1  -1 Machine2 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""

# Process datasets
./gbr_single_category.sh -0 Machine3        -1 Machine4 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""
./gbr_single_category.sh -0 Machine1        -1 Machine3 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""
./gbr_single_category.sh -0 Machine1        -1 Machine4 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""
./gbr_single_category.sh -0 Machine2        -1 Machine3 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""
./gbr_single_category.sh -0 Machine2        -1 Machine4 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""
./gbr_single_category.sh -0 Machine3        -1 Machine4 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES
echo ""
