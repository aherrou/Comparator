faustfile       ?= $(wildcard *.dsp)

all : fixed.cpp float.cpp out

fixed.cpp : $(faustfile)
	faust -a minimal-fixed-point_nomain.cpp -os -fx -cn fxdsp $(faustfile) -o fixed.cpp
float.cpp : $(faustfile)
	faust -a minimal_nomain.cpp -cn fldsp $(faustfile) -o float.cpp
out : compclass.cpp
	c++ -std=c++11 -g compclass.cpp `pkg-config --cflags --libs jack gtk+-2.0` -o out
clean : 
	rm -f fixed.cpp float.cpp out
