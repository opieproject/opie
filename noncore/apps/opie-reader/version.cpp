#include "version.h"

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
/*
	QMessageBox::warning(NULL, PROGNAME,
			     "This is the first time that you have\n"
			     "run this version of OpieReader.\n\n"
			     "There are two new icons visible at\n"
			     "the left end of the toolbar. The left\n"
			     "one brings up the menus, the next\n"
			     "one brings up the settings dialog.\n\n"
			     "Start by tapping the settings icon\n"
			     "and selecting the Buttons tab to\n"
			     "make sure that the buttons are\n"
			     "mapped as you expect\n\n"
			     "Next go to Settings/Toolbars via the\n"
			     "menu icon to set up your toolbars.");
*/
	return true;
    }
    else
    {
	return false;
    }
}
