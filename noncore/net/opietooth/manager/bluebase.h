
#ifndef BLUEBASE_H
#define BLUEBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>
#include <qlist.h>
#include <qpixmap.h>

#include "bluetoothbase.h"

#include "btserviceitem.h"
#include "btdeviceitem.h"
#include "popuphelper.h"

#include "bticonloader.h"

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
        void writeToHciConfig();
	QString status()const;
	void initGui();
        void setInfo();
        PopupHelper m_popHelper;
        Manager *m_localDevice;
        QMap<QString,BTDeviceItem*> m_deviceList;

        void deviceActive( const RemoteDevice &device );

	QString m_deviceName;
	QString m_defaultPasskey;
	bool m_useEncryption;
	bool m_enableAuthentification;
	bool m_enablePagescan;
	bool m_enableInquiryscan;

        QPixmap m_offPix;
        QPixmap m_onPix;

        BTIconLoader *m_iconLoader;

	private slots:
        void addSearchedDevices( const QValueList<RemoteDevice> &newDevices );
        void addServicesToDevice( BTDeviceItem *item );
        void addServicesToDevice( const QString& device, Services::ValueList );
        void addConnectedDevices();
        void addConnectedDevices( ConnectionState::ValueList );
        void startServiceActionClicked( QListViewItem *item );
        void startServiceActionHold( QListViewItem *, const QPoint &, int );
        void deviceActive( const QString& mac, bool connected  );
        void applyConfigChanges();

    };

}

#endif
