//#include <klocale.h>
#include "othemestyle.h"

extern "C"
{
	QStyle * allocate() {
		return new OThemeStyle ( "" );
	}
	int minor_version() {
		return 0;
	}
	int major_version() {
		return 1;
	}
	const char * description() {
		return ( "Theme Style" );
	}
}

