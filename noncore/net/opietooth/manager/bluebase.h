
#ifndef BLUEBASE_H
#define BLUEBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>
#include <qlist.h>
#include <qpixmap.h>

#include "bluetoothbase.h"
#include "btlistitem.h"

#include <remotedevice.h>
#include <manager.h>

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
        void readSavedDevices();
        void writeSavedDevices();
	QString getStatus();
	void initGui();
        void setInfo();
        Manager *localDevice;
        QMap<QString,BTListItem*> deviceList;

        void deviceActive( RemoteDevice *device );

	QString deviceName;
	QString defaultPasskey;
	int useEncryption;
	int enableAuthentification;
	int enablePagescan;
	int enableInquiryscan;

        QPixmap offPix;
        QPixmap onPix;

	private slots:
        void addSearchedDevices( QList<RemoteDevice> &newDevices );
        void addServicesToDevice( BTListItem *item );
        void addServicesToDevice( const QString& device, Services::ValueList );
        void addConnectedDevices();
        void startServiceActionClicked( QListViewItem *item );
        void startServiceActionHold( BTListItem *, const QPoint &, int );
        void deviceActive( const QString& mac, bool connected  );
        void applyConfigChanges();

    };

}

#endif
