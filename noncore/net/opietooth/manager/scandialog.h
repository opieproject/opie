/* main.cpp
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qlist.h>

class QVBoxLayout;
class QGridLayout;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QProgressBar;

namespace Opie {
namespace Bluez {
    class OBluetooth;
    class OBluetoothInterface;
    class OBluetoothDevice;
}
}

using namespace Opie::Bluez;

namespace OpieTooth {

class RemoteDevice;

    class ScanDialog : public QDialog {
        Q_OBJECT

    public:
        ScanDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
        ~ScanDialog();

    private:
        QProgressBar* progress;
        QPushButton* StartStopButton;
        QListView* serviceView;

    public slots:
         void accept();
         void reject();

    protected:
        QVBoxLayout* Layout11;

    private slots:
        void stopSearch();
        void startSearch();
        void progressTimer();
        void deviceFound( const OBluetoothDevice* );
        void propertyChanged( const QString& );
        void defaultInterfaceChanged( OBluetoothInterface * );

    private:
        bool m_search:1;
        int m_progressStat;
        OBluetooth *m_bluetooth;
        OBluetoothInterface *m_btinterface;
        QStringList m_added;

        void emitToManager();
        void connectInterface();
        void searchStopped();

    signals:
        void selectedDevices( const QValueList<RemoteDevice>& );
    };


}

#endif // SCANDIALOG_H
