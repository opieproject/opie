#include <qpe/qpeapplication.h>
#include "backgammon.h"


int main( int argc, char** argv )
{
	QPEApplication app( argc, argv );

	BackGammon* view= new BackGammon();
	app.showMainWidget(view);

	return app.exec();
}

