#include "objects.h"

void Library::load_pix() {
	int i;
        char *name[] = {"wingdows", "apple", "next", "sgi", "sun", "os2",
		"bsd", "linux", "redhat", "hurd"};
	for (i=0; i <= NUM_OS; i++) {
		os[i].load(name[i]);
		if (i) cursor[i].load(name[i], cursor[i].OWN_MASK);
	}
	width = os[0].width;
	height = os[0].height;
}
