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

#include "remotedevice.h"
#include "manager_base.h"

class QVBoxLayout;
class QGridLayout;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QProgressBar;


namespace OpieTooth {

class Manager;
class Device;

    class ScanDialog : public QDialog {
        Q_OBJECT

    public:
        ScanDialog( Manager_Base* manager, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
        ~ScanDialog();

    private: 
	QProgressBar* progress;
        QPushButton* StartStopButton;
        QListView* ListView1;

    public slots:
         void accept();

    protected:
        QVBoxLayout* Layout11;

    private slots:
        void stopSearch();
        void startSearch();
        void progressTimer();
        void fillList( const QString& device, RemoteDevice::ValueList list );

    private:
        bool m_search:1;
        void emitToManager();
        Manager_Base* localDevice;
        int progressStat;

    signals:
        void selectedDevices( const QValueList<RemoteDevice>& );
    };


}

#endif // SCANDIALOG_H
