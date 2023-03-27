#!/usr/bin/env bash

source format.sh $1

rm -rf build
cmake -S . -B build -D TARGET_GROUP=$1
cmake --build build --parallel 8

if [[ $2 == "run" ]]; then
  source run.sh $1
fi