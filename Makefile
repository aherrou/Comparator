faustfile       ?= $(wildcard *.dsp)

all : fixed.cpp float.cpp out

fixed.cpp : $(faustfile)
	faust -a minimal-fixed-point_nomain.cpp -fx -fx-size -1 -cn fxdsp $(faustfile) -o fixed.cpp
float.cpp : $(faustfile)
	faust -a minimal_nomain.cpp -cn fldsp $(faustfile) -o float.cpp
out : compclass.cpp fixed.cpp float.cpp
	c++ -std=c++11 -o3 -g compclass.cpp  -I/usr/local/include/ap_fixed `pkg-config --cflags --static --libs sndfile gtk+-2.0 jack` -o out
clean : 
	rm -f fixed.cpp float.cpp out
