#include <stdio.h>
#include <qpe/qpeapplication.h>
#include <qlayout.h>
#include <qmainwindow.h>

#include "cfgdlg.h"

int main( int argc, char **argv ) {
	QPEApplication app(argc, argv);
	CfgFile cfile;
	CfgParser cp;
	cp.load(QPEApplication::qpeDir()+"/share/zkb/zkb.xml", cfile);


	CfgDlg c(0, &cfile, &app, true);


	app.showMainWidget(&c);
	int ret = QPEApplication::execDialog(&c);

	return ret;
}
