#include <stdio.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qmainwindow.h>

#include "cfgfile.h"
#include "cfgdlg.h"

int main( int argc, char **argv ) {
	QPEApplication app(argc, argv);
	CfgFile cfile;
	CfgParser cp;
	cp.load(QPEApplication::qpeDir()+"/share/zkb/zkb.xml", cfile);

	QMainWindow m;
	CfgDlg c(&m, &cfile, &app);
	app.showMainWidget(&m);
	m.hide();
	c.showMaximized();

	int ret = app.exec();

	return ret;
}
