#!/usr/bin/env bash

# Compile and execute the signal pre-processor
# Compile and execute the signal neural network trainer
# Compile and execute the signal classifier

NETWORK_INPUT_SAMPLE_LENGTH=128
EPOCH_COUNT=5
NETWORK_HIDDEN_LAYER_NODES=40

helpFlag=0
compileFlag=0
scatterDiagramFlag=0
confusionMatrixFlag=0
preProcessDataFlag=0
debugFlag=0

InputFile_0="Machine1"
InputFile_1="Machine2"
InputFile_2="Machine3"
InputFile_3="Machine4"


while getopts 0:1:2:3:cpdmsl:e:H:h flag
do
    case "${flag}" in
        1) InputFile_0=${OPTARG};;
        1) InputFile_1=${OPTARG};;
        1) InputFile_2=${OPTARG};;
        1) InputFile_3=${OPTARG};;
        c) compileFlag=1;;
        p) preProcessDataFlag=1;;
        d) debugFlag=1;;
        m) confusionMatrixFlag=1;;
        s) scatterDiagramFlag=1;;
        l) NETWORK_INPUT_SAMPLE_LENGTH=${OPTARG};;
        e) EPOCH_COUNT=${OPTARG};;
        H) NETWORK_HIDDEN_LAYER_NODES=${OPTARG};;
        h) helpFlag=1;;
    esac
done

echo "InputFile_0        : $InputFile_0"
echo "InputFile_1        : $InputFile_1"
echo "InputFile_2        : $InputFile_2"
echo "InputFile_3        : $InputFile_3"
echo "compileFlag        : $compileFlag"
echo "preProcessDataFlag : $preProcessDataFlag"
echo "debugFlag          : $debugFlag"
echo "confusionMatrixFlag: $confusionMatrixFlag"
echo "scatterDiagramFlag : $scatterDiagramFlag"
echo "NETWORK_INPUT_SAMPLE_LENGTH      : $NETWORK_INPUT_SAMPLE_LENGTH"
echo "EPOCH_COUNT        : $EPOCH_COUNT"
echo "NETWORK_HIDDEN_LAYER_NODES : $NETWORK_HIDDEN_LAYER_NODES"

if [ $helpFlag == 1 ]
then
    echo " "
    echo "process.sh"
    echo "    -h: This help screen"
    echo "    -0: File index #0"
    echo "    -1: File index #1"
    echo "    -2: File index #2"
    echo "    -3: File index #3"
    echo "    -c: Compile the source files"
    echo "    -p: Pre-process the original recordings, otherwise use cached .csv data"
    echo "    -l: Sample length: Supported lengths: 64, 128, 256 etc. (Default=128)"
    echo "    -e: Epoch count (Default=5)"
    echo "    -H: Number of hidden layer nodes (Default=25)"
    echo "    -s: Enable scatter diagram"
    echo "    -m: Enable confusion matrix diagram"
    exit
fi


if [ "$preProcessDataFlag" == 1 ]; then                     # If pre-processing is enabled
    if [ $compileFlag == 1 ]; then                          # If compilation is enabled
        if [ -f preprocess_wav ]; then
            rm -f preprocess_wav
        fi
                                                            # Build the .wav pre-processing file
        gcc preprocess_wav.c -O3 -Wall -DNETWORK_INPUT_SAMPLE_LENGTH=$NETWORK_INPUT_SAMPLE_LENGTH -Wno-main -Wno-unused-value -std=c99 -l siglib -l gnuplot_c -o preprocess_wav -lm
    fi

    if [ -f preprocess_wav ]; then
        rm -rf PreProcessed/*
        ./preprocess_wav $InputFile_0 0
        ./preprocess_wav $InputFile_1 1
        ./preprocess_wav $InputFile_2 2
        ./preprocess_wav $InputFile_3 3
    fi
    rm -f trainLength_*.txt
    touch trainLength_$NETWORK_INPUT_SAMPLE_LENGTH.txt
else
    if [ ! -f trainLength_$NETWORK_INPUT_SAMPLE_LENGTH.txt ]; then
        echo " "
        echo "Pre-processed data not of desired length !!"
        echo "Re-run process.sh with -p option and the desired sample length."
        echo " "
        exit
    fi
fi


cat "PreProcessed/Training_"$InputFile_0"_0.csv" > Tdata.csv
cat "PreProcessed/Training_"$InputFile_1"_1.csv" >> Tdata.csv
cat "PreProcessed/Training_"$InputFile_2"_2.csv" >> Tdata.csv
cat "PreProcessed/Training_"$InputFile_3"_3.csv" >> Tdata.csv

cat "PreProcessed/Validation_"$InputFile_0"_0.csv" > Vdata.csv
cat "PreProcessed/Validation_"$InputFile_1"_1.csv" >> Vdata.csv
cat "PreProcessed/Validation_"$InputFile_2"_2.csv" >> Vdata.csv
cat "PreProcessed/Validation_"$InputFile_3"_3.csv" >> Vdata.csv


if [ $compileFlag == 1 ]; then                              # If compilation is enabled
    if [ -f network_train_multi_category ]; then
        rm -f network_train_multi_category
    fi
                                                            # Train the network
    gcc network_train_multi_category.c -O3 -Wall -DNETWORK_INPUT_SAMPLE_LENGTH=$NETWORK_INPUT_SAMPLE_LENGTH -DNETWORK_HIDDEN_LAYER_NODES=$NETWORK_HIDDEN_LAYER_NODES -DACTIVATION_TYPE="SIGLIB_ACTIVATION_TYPE_LOGISTIC" -Wno-main -Wno-unused-value -std=gnu11 -l siglib -l gnuplot_c -o network_train_multi_category -lm
fi

if [ -f network_train_multi_category ]; then
    if [ "$debugFlag" == 1 ]; then                          # debugFlag" == 1
        echo EXECUTING CONFUSION MATRIX AND DEBUG
        ./network_train_multi_category -0 $InputFile_0 -1 $InputFile_1 -2 $InputFile_2 -3 $InputFile_3 -e $EPOCH_COUNT -d
    else
        ./network_train_multi_category -0 $InputFile_0 -1 $InputFile_1 -2 $InputFile_2 -3 $InputFile_3 -e $EPOCH_COUNT
    fi
else
    exit
fi

if [ $compileFlag == 1 ]; then                              # If compilation is enabled
    if [ -f network_validate_multi_category ]; then
        rm -f network_validate_multi_category
    fi
                                                            # Validate the network
    gcc network_validate_multi_category.c -O3 -Wall -DNETWORK_INPUT_SAMPLE_LENGTH=$NETWORK_INPUT_SAMPLE_LENGTH -DNETWORK_HIDDEN_LAYER_NODES=$NETWORK_HIDDEN_LAYER_NODES -DACTIVATION_TYPE="SIGLIB_ACTIVATION_TYPE_LOGISTIC" -Wno-main -Wno-unused-value -std=gnu11 -l siglib -l gnuplot_c -o network_validate_multi_category -lm
fi

# Epoch count in validation only used for graph titles
if [ -f network_validate_multi_category ]; then
    if [ "$debugFlag" == 1 ] && [ "$confusionMatrixFlag" == 1 ]; then       # Debug && ConfusionMatrix
        echo EXECUTING CONFUSION MATRIX AND DEBUG
        ./network_validate_multi_category -0 $InputFile_0 -1 $InputFile_1 -2 $InputFile_2 -3 $InputFile_3 -e $EPOCH_COUNT -d -m
    elif [ "$debugFlag" == 1 ]; then                        # debugFlag" == 1
        echo EXECUTING DEBUG
        ./network_validate_multi_category -0 $InputFile_0 -1 $InputFile_1 -2 $InputFile_2 -3 $InputFile_3 -e $EPOCH_COUNT -d
    elif [ "$confusionMatrixFlag" == 1 ]; then              # confusionMatrixFlag" == 1
        echo EXECUTING CONFUSION MATRIX
        ./network_validate_multi_category -0 $InputFile_0 -1 $InputFile_1 -2 $InputFile_2 -3 $InputFile_3 -e $EPOCH_COUNT -m
    else
        echo EXECUTING NO DEBUG
        ./network_validate_multi_category -0 $InputFile_0 -1 $InputFile_1 -2 $InputFile_2 -3 $InputFile_3 -e $EPOCH_COUNT
     fi
fi

exit
