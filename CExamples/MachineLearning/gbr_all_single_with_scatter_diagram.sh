#!/usr/bin/env bash

NETWORK_INPUT_SAMPLE_LENGTH=128
EPOCH_COUNT=5
NETWORK_HIDDEN_LAYER_NODES=25

# Compare all modes and plot graphs
echo  Model Accuracy: Configuration: -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES

# Pre-process data on first comparison
./gbr_single_category.sh -p -0 Machine4     -1 Machine2 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""

# Process neural networ
./gbr_single_category.sh -0 Machine4        -1 Machine2 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""
./gbr_single_category.sh -0 Machine1        -1 Machine2 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""
./gbr_single_category.sh -0 Machine1        -1 Machine3 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""
./gbr_single_category.sh -0 Machine1        -1 Machine5 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""
./gbr_single_category.sh -0 Machine2        -1 Machine3 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""
./gbr_single_category.sh -0 Machine2        -1 Machine5 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""
./gbr_single_category.sh -0 Machine3        -1 Machine5 -l $NETWORK_INPUT_SAMPLE_LENGTH -e $EPOCH_COUNT -H $NETWORK_HIDDEN_LAYER_NODES -s
echo ""
