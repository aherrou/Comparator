faustfile       ?= $(wildcard *.dsp)
gtk_pkg = -I/opt/local/include/gtk-2.0 -I/opt/local/lib/gtk-2.0/include -I/opt/local/include/pango-1.0 -I/opt/local/include/gio-unix-2.0 -I/opt/local/include -I/opt/local/include/cairo -I/opt/local/include/atk-1.0 -I/opt/local/include/gdk-pixbuf-2.0 -I/opt/local/include -I/opt/local/include/pango-1.0 -I/opt/local/include/harfbuzz -I/opt/local/include/pango-1.0 -I/opt/local/include -I/opt/local/include/fribidi -I/opt/local/include -I/opt/local/include/cairo -I/opt/local/include/pixman-1 -I/opt/local/include -I/opt/local/include/harfbuzz -I/opt/local/include/glib-2.0 -I/opt/local/lib/glib-2.0/include -I/opt/local/include -I/opt/local/include/freetype2 -I/opt/local/include/libpng16 -I/opt/local/include -L/opt/local/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -lXrender -lXinerama -lXi -lXrandr -lXcursor -lXcomposite -lXdamage -lXfixes -lX11 -lXext -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lintl -lharfbuzz -lfontconfig -lfreetype

all : fixed.cpp float.cpp out

fixed.cpp : $(faustfile)
	faust -a minimal-fixed-point_nomain.cpp -os -fx -cn fxdsp $(faustfile) -o fixed.cpp
float.cpp : $(faustfile)
	faust -a minimal_nomain.cpp -cn fldsp $(faustfile) -o float.cpp
out : compclass.cpp
	c++ -std=c++11 -o3 -g compclass.cpp  -I/usr/local/include/ap_fixed `pkg-config --cflags --libs jack` $(gtk_pkg) -o out
clean : 
	rm -f fixed.cpp float.cpp out
