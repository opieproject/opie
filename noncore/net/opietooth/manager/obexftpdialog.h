/* $Id: obexftpdialog.h,v 1.2 2006-03-20 21:44:55 korovkin Exp $ */
/* OBEX file browser dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
 * This code uses and is based on ObexFTP project code: http://triq.net/obexftp/
 */
#ifndef _OBEXFTPDIALOG_H_
#define _OBEXFTPDIALOG_H_
#include "obexftpdialogbase.h"
#include <qstring.h>
#include <obexftp.h>
#include <uuid.h>
#include <client.h>
#include <opie2/ofileselector.h>
#include <qlayout.h>
namespace OpieTooth {
    class ObexFtpDialog : public ObexFtpDialogBase {
        Q_OBJECT
    public:
        ObexFtpDialog(const QString& device = 0, int port = 0, 
            QWidget* parent = 0, const char* name = 0, bool modal = TRUE, 
            WFlags fl = 0);
        ~ObexFtpDialog();
        void log(QString str);
        void log(QCString str);
        void log(const char* str);
        void log(QString& str);
        void incProgress();
        void doneProgress();
    protected:    
        int cli_connect_uuid(const uint8_t *uuid, int uuid_len);
        int errBox(QCString msg); //Error message box
        int errBox(QString msg); //Error message box
        int errBox(const char* msg); //Error message box
        void status(QCString msg); //Text in the status bar
        void status(QString msg); //Text in the status bar
        void status(const char* msg); //Text in the status bar
    protected:
        QString m_device; //device MAC address
        int m_port; //port
        int transport; //transport type
        bool use_conn; 
        bool use_path; 
        obexftp_client_t* client; //Obex ftp client handler
        QString curdir; //Current directory on device
        QString localCurdir; //Local current directory
        QString file2get; //Remote file name
        QString local; //Local file name
        int progressStatus; //Progress status
        Opie::Ui::OFileSelector* destFile; //Destination file or directory
        QVBoxLayout* localLayout; //Window layout
        int nRetries; //Number of retries (on connection)
    private slots:
        void slotBrowse();
        void slotCd(QListViewItem* item);
        void getFile();    
        void putFile();    
        void updateDir(const QString& newdir);
    };
};
#endif
