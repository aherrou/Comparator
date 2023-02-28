# Comparator
fixedpoint/float faust compilation comparator


* faust -a minimal-fixed-point.cpp -fx -cn fxdsp phasor.dsp -o fixed.cpp
* faust -a minimal.cpp -cn fldsp phasor.dsp -o float.cpp
* remove main function part from the files and remove `std::` in `std::exp10`
* c++ -std=c++11 -g compclass.cpp `pkg-config --cflags --libs jack $OCVLIBS gtk+-2.0` -o test
* ./test

