
#ifndef BLUEBASE_H
#define BLUEBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>
#include <qlist.h>

#include "bluetoothbase.h"

#include <remotedevice.h>

class QVBox;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QPushButton;
class QTabWidget;
class QCheckBox;

namespace OpieTooth {



    class BlueBase : public BluetoothBase {
        Q_OBJECT

    public:
        BlueBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
        ~BlueBase();

    protected:


            private slots:
        void startScan();

    private:
	void readConfig();
	void writeConfig();
	QString getStatus();
	void initGui();


	QString deviceName;
	QString defaultPasskey;
	int useEncryption;
	int enableAuthentification;
	int enablePagescan;
	int enableInquiryscan;

	private slots:
        void addSearchedDevices( QList<RemoteDevice> &newDevices );
      	void applyConfigChanges();

    };

}

#endif
