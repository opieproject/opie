/***************************************************************************
                          NetworkDialog.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include <qdialog.h>
#include <qurloperator.h>
#include <qcstring.h>
#include <qfile.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringlist.h>

//#include <qtimer.h>

//class QProgressBar;

class NetworkDialog : public QDialog
{ 
    Q_OBJECT

public:
    NetworkDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, const QStringList netList=0);
/*     NetworkDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0,  const QString & UrlStr = 0,  const QString & dirStr=0,const QString & fileStr=0, const QString & psrtialFile=0); */
    ~NetworkDialog();
    
    QFile fiole;
    QTimer * connectionTimer;
    QTimer * timer;
		int timerProgess;
		bool posTimer; 
    QString s_partialFileName, s_finally;
    bool foundIt, successDownload;
    QLabel * warnLabel;
    QLabel* TextLabel3;
    QPushButton* buttonHelp;
    bool ok;
    QString local_library;
    QString ftp_host;
    QString ftp_base_dir;
    QString dir, strUrl, networkUrl;   
    
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QString localFileName;
    bool fileFound;
    int i, totalBytesDownloaded;
		bool autoOk;

		void initDialog();

protected:
    QHBoxLayout* hbox;

private slots:
		bool downloadFile( QString strUrl);
		void connectionTimeSlot();
		void timeSlot();
		void doOk();
   
	private:
/*  #ifndef Q_WS_QWS */

/*      QString getOpenFileName(); */
/*      QUrlOperator op; */
/*  #endif */
};

#endif // NETWORKDIALOG_H
