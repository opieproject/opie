#include <qpe/qpeapplication.h>
#include "qcheckbook.h"

int main(int argc, char **argv)
{
	QPEApplication app(argc, argv);
	QCheckBook *qcb = new QCheckBook();
	app.setMainWidget(qcb);
	qcb->showMaximized();
	return app.exec();
}
