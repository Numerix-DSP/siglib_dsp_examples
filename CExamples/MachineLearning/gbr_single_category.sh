#!/usr/bin/env bash

# Compile and execute the signal pre-processor
# Compile and execute the signal neural network trainer
# Compile and execute the signal classifier

NETWORK_INPUT_SAMPLE_LENGTH=128
EPOCH_COUNT=5
NETWORK_HIDDEN_LAYER_NODES=25

dataAugmentationStride=$NETWORK_INPUT_SAMPLE_LENGTH     # No overlap of frames = no data augmentation
helpFlag=0
compileFlag=0
scatterDiagramFlag=0
confusionMatrixFlag=0
preProcessDataFlag=0
debugFlag=0

InputFile_0="Machine4"
InputFile_1="Machine3"

while getopts 0:1:cpdmsl:a:e:H:h flag
do
    case "${flag}" in
        0) InputFile_0=${OPTARG};;
        1) InputFile_1=${OPTARG};;
        a) dataAugmentationStride=${OPTARG};;
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

echo "InputFile_0                : $InputFile_0"
echo "InputFile_1                : $InputFile_1"
echo "dataAugmentationStride      : $dataAugmentationStride"
echo "compileFlag                : $compileFlag"
echo "preProcessDataFlag         : $preProcessDataFlag"
echo "debugFlag                  : $debugFlag"
echo "confusionMatrixFlag        : $confusionMatrixFlag"
echo "scatterDiagramFlag         : $scatterDiagramFlag"
echo "NETWORK_INPUT_SAMPLE_LENGTH: $NETWORK_INPUT_SAMPLE_LENGTH"
echo "EPOCH_COUNT                : $EPOCH_COUNT"
echo "NETWORK_HIDDEN_LAYER_NODES : $NETWORK_HIDDEN_LAYER_NODES"

if [ $helpFlag == 1 ]
then
    echo " "
    echo "process.sh"
    echo "    -h: This help screen"
    echo "    -0: File index #0"
    echo "    -1: File index #1"
    echo "    -a: Data augmentation depth"
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
    rm -rf PreProcessed/*

    if [ $compileFlag == 1 ]; then                          # If compilation is enabled
        if [ -f preprocess_wav ]; then
            rm -f preprocess_wav
        fi
                                                            # Build the .wav pre-processing file
        gcc preprocess_wav.c -O3 -Wall -DNETWORK_INPUT_SAMPLE_LENGTH=$NETWORK_INPUT_SAMPLE_LENGTH -Wno-main -Wno-unused-value -std=c99 -l siglib -l gnuplot_c -o preprocess_wav -lm
    fi

    if [ -f preprocess_wav ]; then
        for file in Original/*.wav
        do
            # Extract file base name
            filename=${file##*/}
            fileext=".wav"
            filename_short=${filename//$fileext/}
            echo "Executing preprocess_wav"
            ./preprocess_wav "${filename//$fileext/}" 0 $dataAugmentationStride
            ./preprocess_wav "${filename//$fileext/}" 1 $dataAugmentationStride
        done
        rm -f trainLength_*.txt
        touch trainLength_$NETWORK_INPUT_SAMPLE_LENGTH.txt
    fi
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
cat "PreProcessed/Validation_"$InputFile_0"_0.csv" > Vdata.csv
cat "PreProcessed/Training_"$InputFile_1"_1.csv" >> Tdata.csv
cat "PreProcessed/Validation_"$InputFile_1"_1.csv" >> Vdata.csv

if [ $compileFlag == 1 ]; then                              # If compilation is enabled
    if [ -f network_train_single_category ]; then
        rm -f network_train_single_category
    fi
                                                            # Train the network
    gcc network_train_single_category.c -O3 -Wall -DNETWORK_INPUT_SAMPLE_LENGTH=$NETWORK_INPUT_SAMPLE_LENGTH -DNETWORK_HIDDEN_LAYER_NODES=$NETWORK_HIDDEN_LAYER_NODES -DACTIVATION_TYPE="SIGLIB_ACTIVATION_TYPE_LOGISTIC" -Wno-main -Wno-unused-value -std=gnu11 -l siglib -l gnuplot_c -o network_train_single_category -lm
fi
if [ -f network_train_single_category ]; then
    ./network_train_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT
else
    exit
fi

if [ $compileFlag == 1 ]; then                              # If compilation is enabled
    if [ -f network_validate_single_category ]; then
        rm -f network_validate_single_category
    fi
                                                            # Validate the network
    gcc network_validate_single_category.c -O3 -Wall -DNETWORK_INPUT_SAMPLE_LENGTH=$NETWORK_INPUT_SAMPLE_LENGTH -DNETWORK_HIDDEN_LAYER_NODES=$NETWORK_HIDDEN_LAYER_NODES -DACTIVATION_TYPE="SIGLIB_ACTIVATION_TYPE_LOGISTIC" -Wno-main -Wno-unused-value -std=gnu11 -l siglib -l gnuplot_c -o network_validate_single_category -lm
fi

# Epoch count in validation only used for graph titles
if [ -f network_validate_single_category ]; then
    if [ "$debugFlag" == 1 ] && [ "$confusionMatrixFlag" == 1 ] && [ "$scatterDiagramFlag" == 1 ]; then     # Debug && ConfusionMatrix && ScatterDiagram
        ./network_validate_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT -d -m -s
    elif [ "$debugFlag" == 1 ] && [ "$confusionMatrixFlag" == 1 ]; then             # Debug && ConfusionMatrix
        ./network_validate_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT -d -m
    elif [ "$debugFlag" == 1 ] && [ "$scatterDiagramFlag" == 1 ]; then              # Debug && ScatterDiagram
        ./network_validate_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT -d -s
    elif [ "$confusionMatrixFlag" == 1 ] && [ "$scatterDiagramFlag" == 1 ]; then    # ConfusionMatrix && ScatterDiagram
        ./network_validate_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT -m -s
    elif [ "$confusionMatrixFlag" == 1 ]; then                                      # ConfusionMatrix
        ./network_validate_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT -m
    elif [ "$scatterDiagramFlag" == 1 ]; then                                       # ScatterDiagram
        ./network_validate_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT -s
    else
        ./network_validate_single_category -0 $InputFile_0 -1 $InputFile_1 -e $EPOCH_COUNT
    fi
fi

