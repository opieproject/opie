
#include <qpe/resource.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qstring.h>
#include <qpopupmenu.h>

class  QVBox;

class Oxygen : public QMainWindow
{
	Q_OBJECT
	
    public:
		Oxygen();
        QVBox *vbox;
	private slots:
        void slotCalculations();
        void slotSettings();
        void slotPSE();
		void close();
};
