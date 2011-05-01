CFLAGS = `pkg-config --cflags libxml-2.0 glib-2.0`
LIBS = `pkg-config --libs libxml-2.0 glib-2.0`

default: dpconvert

dpconvert: dpconvert.c
	gcc $(CFLAGS) $(LIBS) -o dpconvert dpconvert.c

clean:
	rm dpconvert
