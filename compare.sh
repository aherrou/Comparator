#! /bin/bash

file=$1
./faust2comparator file
binary="${file%.dsp}"
./$binary -lw
