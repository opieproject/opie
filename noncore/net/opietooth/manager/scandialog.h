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
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QProgressBar;


namespace OpieTooth {

#include <remotedevices.h>
class Manager;
class Device;

    class ScanDialog : public QDialog {
        Q_OBJECT

    public:
        ScanDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
        ~ScanDialog();

        QFrame* Frame7;
        QProgressBar* progress;
        QPushButton* StartButton;
        QPushButton* StopButton;
        QListView* ListView1;


    protected:
        QVBoxLayout* Layout11;

    private slots:
        void stopSearch();
        void startSearch();
          void progressTimer();
        void fillList(const QString& device, RemoteDevices::ValueList list);

    private:
        void emitToManager();
        Manager *localDevice;
        int progressStat;

    signals:
        void selectedDevices(QList<RemoteDevices>&);
    };


}

#endif // SCANDIALOG_H
