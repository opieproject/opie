#include "version.h"
#include "names.h"
#include <qmessagebox.h>

bool CheckVersion(int& major, int& bkmktype, char& minor)
{
    if (
	(major != MAJOR)
	||
	(bkmktype != BKMKTYPE)
	||
	(minor != MINOR)
	)
    {
	major = MAJOR;
	bkmktype = BKMKTYPE;
	minor = MINOR;

	QMessageBox::warning(NULL, PROGNAME,
			     "New Features in this version\n\n\n"
			     "Relative margin settings\n"
			     "Cyrillic hyphenation");
	return true;
    }
    else
    {
	return false;
    }
}
