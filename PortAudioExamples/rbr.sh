#!/usr/bin/env bash

# Shell script for building and executing application with GCC

rm -f $1

./rb.sh $1

if [ -f $1 ]
then
    ./$1 $2 $3 $4 $5 $6 $7 $8 $9 $10
fi
