#!/bin/bash

rm -rf build
rm LR1

mkdir build
cd build
cmake .. && make

