#! /bin/bash

file=$1
./faust2comparator $file
binary="${file%.dsp}"
./$binary -lw
afplay "float-$binary.wav"
afplay "fixed-$binary.wav"
