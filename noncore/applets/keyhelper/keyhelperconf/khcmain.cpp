#include <qpe/qpeapplication.h>
#include "KHCWidget.h"

int main(int argc, char* argv[])
{
	QPEApplication a(argc, argv);

	KHCWidget w;
	a.showMainWidget(&w);
	return a.exec();
}
