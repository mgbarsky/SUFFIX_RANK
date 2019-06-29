#!/bin/bash

#program parameters
RANK_DIR=$1
OUTPUT_DIR=$2
TEMP_DIR=TMP


SUCCESS=0
FAILURE=1
EMPTY=2

STATE=$SUCCESS
CHUNKS=0

TRUESTART=$(gdate +%s.%N)

CHUNKS=$(($(ls -l ${RANK_DIR}/* | wc -l)/2))

#need tmp dir
if [[ -d $TEMP_DIR ]]
then
    rm -rf ${TEMP_DIR}/*
else
    mkdir ${TEMP_DIR}
fi

START=$(gdate +%s.%N)
./create_pairs ${RANK_DIR} ${TEMP_DIR} $CHUNKS
STATUS=$?

if [[ $STATUS -eq $FAILURE ]]
then
    exit 1
fi
echo "finished creating pairs"
DUR=$(echo "$(gdate +%s.%N) - $START" | bc)
printf "Created in %.4f seconds\n" $DUR

START=$(gdate +%s.%N)


./invert ${TEMP_DIR} ${OUTPUT_DIR} $CHUNKS
STATUS=$?

if [[ $STATUS -eq $FAILURE ]]
then
    exit 1
fi
DUR=$(echo "$(gdate +%s.%N) - $START" | bc)
printf "Inverted in %.4f seconds\n" $DUR

#clean temp directory 
rm -rf ${TEMP_DIR}/*

exit 0
