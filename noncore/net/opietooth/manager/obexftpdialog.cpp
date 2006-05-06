/* $Id: obexftpdialog.cpp,v 1.6 2006-05-06 18:37:33 korovkin Exp $ */
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qlayout.h>
#include <errno.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include "obexftpdialog.h"
#include "filelistitem.h"

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <opie2/odebug.h>
#include <opie2/ofileselector.h>
#include <opie2/ofiledialog.h>

using namespace Opie::Core;
using namespace Opie::Ui;

using namespace OpieTooth;

#define MAX_PROGRESS 14 //Maximal progress bar

static void info_cb(int event, const char *msg, int len, void* data);

/*
 * Public constructor
 * device - bluetooth address of the device
 * port - port to connect to
 */
ObexFtpDialog::ObexFtpDialog(const QString& device, int port, 
    QWidget* parent, const char* name, bool modal, WFlags fl) 
    : ObexFtpDialogBase(parent, name, modal, fl), m_device(device), 
    m_port(port), curdir("")
{
    client = NULL;
    nRetries = 3;
    transport = OBEX_TRANS_BLUETOOTH;
    use_conn = TRUE;
    use_path = TRUE;
    progressStatus = 0;
    localCurdir = "/";
    browseLog->setEdited(FALSE);
    fileList->setSorting(1);
    fileList->clear();
    fileProgress->setTotalSteps(MAX_PROGRESS);
    statusBar->clear();
    localLayout = new QVBoxLayout(localFs);
    localLayout->setSpacing( 0 );
    localLayout->setMargin( 0 );
    destFile = new OFileSelector(localFs, 
        OFileSelector::FileSelector, 
        OFileSelector::ExtendedAll, localCurdir, "");
    destFile->setCloseVisible(false);
    destFile->setNewVisible(false);
    localLayout->addWidget(destFile);
    nReries->setValue(nRetries);
    connect(browseOK, SIGNAL(clicked()), SLOT(slotBrowse()));
    connect(fileList, SIGNAL(clicked(QListViewItem*)), 
        SLOT(slotCd(QListViewItem*)));
    connect(getButton, 
        SIGNAL(clicked()),
        SLOT(getFile()));
    connect(putButton,
        SIGNAL(clicked()),
        SLOT(putFile()));
    connect(delButton,
        SIGNAL(clicked()),
        SLOT(delFile()));
    connect(destFile, 
        SIGNAL(dirSelected (const QString&)),
        SLOT(updateDir(const QString&)));
    connect(saveButton, 
        SIGNAL(clicked()),
        SLOT(slotSaveLog()));
    connect(browseButton, 
        SIGNAL(clicked()),
        SLOT(slotBrowseLog()));
}

ObexFtpDialog::~ObexFtpDialog()
{
    if (client != NULL) {
        obexftp_disconnect(client);
        obexftp_close(client);
    }
}

void ObexFtpDialog::slotBrowse()
{
    doBrowse(TRUE);
}

/*
 * Do device browsing
 */
void ObexFtpDialog::doBrowse(bool reconnect)
{
    stat_entry_t* ent; //Directory entry
    void *dir; //Directory to read
    const uint8_t* use_uuid; //uuid
    int len; //uuid length
    FileListItem* root; //root node
    int fsize; //file size
    bool numOk; //true if the string is a number 
    int tmp; //just a temp var
    
    status(tr("Connecting to ") + m_device);
    odebug << "Browse device " << m_device << oendl;
    browseLog->clear();
    fileList->clear();
    progressStatus = 0;
    fileProgress->setTotalSteps(MAX_PROGRESS);
    fileProgress->reset();

    //Get parameters
    tmp = nReries->text().toInt(&numOk);
    if (numOk)
        nRetries = tmp;
    if (uuidType->currentText() == "FBS") {
        use_uuid = UUID_FBS;
        len = sizeof(UUID_FBS);
    } else if (uuidType->currentText() == "S45") {
        use_uuid = UUID_S45;
        len = sizeof(UUID_S45);
    } else {
        use_uuid = UUID_FBS;
        len = sizeof(UUID_FBS);
    }

    if (!cli_connect_uuid(use_uuid, len, reconnect)) {
        log(tr("Connection failed: ") + tr(strerror(errno)));
        errBox("Connection failed");
        status("Connection failed");
        return;
    }
    else {
        log(QString("Connected to ") + m_device);
        status(QString("Connected to ") + m_device);
    }
    /* List folder */
    root = new FileListItem(fileList, NULL);
    dir = obexftp_opendir(client, curdir);
	while ((ent = obexftp_readdir(dir)) != NULL) {
        FileListItem* a; //List view item
    	if (ent->mode != 16877) {
            fsize = ent->size;
        }
        else
            fsize = 0;
		log(QString(ent->name) + QString(" ") + 
            QString::number(ent->mode));
        
        a = new FileListItem(fileList, ent, fsize);
	}
	obexftp_closedir(dir);
}

//Error message box
int ObexFtpDialog::errBox(QCString msg)
{
    return QMessageBox::critical(this, tr("ObexFTP error"), msg);
}

int ObexFtpDialog::errBox(QString msg)
{
    return QMessageBox::critical(this, tr("ObexFTP error"), msg);
}

int ObexFtpDialog::errBox(const char* msg)
{
    return QMessageBox::critical(this, tr("ObexFTP error"), tr(msg));
}

//Text in the status bar
void ObexFtpDialog::status(QCString msg)
{
    statusBar->setText(msg);
    statusBar->repaint();
}

void ObexFtpDialog::status(QString msg)
{
    statusBar->setText(msg);
    statusBar->repaint();
}

void ObexFtpDialog::status(const char* msg)
{
    statusBar->setText(msg);
    statusBar->repaint();
}

/*
 * Change directory with item under the cursor
 */
void ObexFtpDialog::slotCd(QListViewItem* item)
{
    FileListItem* file = (FileListItem*)item;
    int idx;
    if (file == NULL)
        return;
    odebug << "Item " << file->text(0) << " clicked" << oendl;
    if (file->gettype() == IS_DIR) {
        if (file->text(0) == "../") {
            if (curdir.right(1) == "/")
                curdir.remove(curdir.length() - 1, 1);
            idx = curdir.findRev('/');
            if (idx >= 0)
                curdir.remove(idx, curdir.length() - idx);
            else
                curdir = "";
        }
        else {
            if (curdir != "" && curdir.right(1) != "/")
                curdir += "/";
            curdir += file->text(0);
        }
        odebug << "Browse " << curdir << oendl;
        if (obexftp_setpath(client, curdir, 0) < 0)
            log(tr("CD failed: ") + tr(strerror(errno)));
        doBrowse();
    }
}

/*
 * Copy file from a remote device to the local device
 */
void ObexFtpDialog::getFile()
{
    FileListItem* file = (FileListItem*)fileList->selectedItem();
    int result;
    if (file == NULL)
        return;
    file2get = "/";
    local = localCurdir;
    if (local == "") {
        errBox("Select a destination first");
        return;
    }
    if (local.right(1) != "/")
        local += "/";
    if (file->gettype() == IS_FILE) {
        if (client == NULL) {
            errBox("No connection established");
            return;
        }
        file2get += curdir;
        if (curdir != "" && curdir.right(1) != "/")
            file2get += "/";
        file2get += file->text(0);
        local += file->text(0);
        odebug << "Copy " << file2get << " to " << local << oendl;
        progressStatus = 0;
        fileProgress->setTotalSteps(file->getsize() / 1024);
        fileProgress->reset();
        status(tr("Receiving file ") + file2get);
        result = obexftp_get(client, local, file2get);
        if (result < 0) {
            log(file2get + QString(" receive ERROR:\n") + tr(strerror(errno))); 
            errBox(file2get + QString(" receive ERROR"));
            status(file2get + QString(" receive ERROR"));
        }
        else {
            log(file2get + QString(" received")); 
            status(file2get + QString(" received")); 
            destFile->reread();
        }
    }
}

/*
 * Copy file from the local device to a remote device
 */
void ObexFtpDialog::putFile()
{
    int result; //OPeration result
    int idx; //Index of a symbol in the string
    struct stat localFStat; //Local file information
    
    if (client == NULL) {
        errBox("No connection established");
        return;
    }
    local = destFile->selectedName();
    if (local == "") {
        errBox("No file slected");
        return;
    }
    result = stat(local, &localFStat);
    if (result < 0) {
        errBox(tr("Wrong file selected ") + local + tr(" ") + 
            tr(strerror(errno)));
        return;
    }
    idx = local.findRev('/');
    if (idx > 0) {
        file2get = local.right(local.length() - idx - 1);
    }
    else 
        file2get = local;
    
    odebug << "Copy " << local << " to " << file2get << oendl;
    progressStatus = 0;
    fileProgress->setTotalSteps(localFStat.st_size / 1024);
    fileProgress->reset();
    status(tr("Sending file ") + local);
    result = obexftp_put_file(client, local, file2get);
    if (result < 0) {
        log(local + QString(" send ERROR:\n") + tr(strerror(errno))); 
        errBox(local + QString(" send ERROR"));
        status(local + QString(" send ERROR"));
    }
    else {
        doBrowse();
        log(local + QString(" sent")); 
        status(local + QString(" sent")); 
    }
}

/*
 * Delete file on a remote device
 */
void ObexFtpDialog::delFile()
{
    FileListItem* file = (FileListItem*)fileList->selectedItem();
    int result;
    if (file == NULL)
        return;
    file2get = "/";
    if (file->gettype() == IS_FILE) {
        if (client == NULL) {
            errBox("No connection established");
            return;
        }
        file2get += curdir;
        if (curdir != "" && curdir.right(1) != "/")
            file2get += "/";
        file2get += file->text(0);
    }
    result = QMessageBox::warning(this, tr("Remove File"), 
        tr("Do you want to remove\n") + file2get, "Yes", "No");
    if (result != 0)
        return;
    odebug << "Remove " << file2get << oendl;
    result = obexftp_del(client, file2get);
    if (result < 0) {
        log(file2get + QString(" remove ERROR\n") + tr(strerror(errno))); 
        errBox(file2get + QString(" remove ERROR"));
        status(file2get + QString(" remove ERROR"));
    }
    else {
        doBrowse();
        log(file2get + QString(" removed")); 
        status(file2get + QString(" removed")); 
    }
}

/* connect with given uuid. re-connect every time */
int ObexFtpDialog::cli_connect_uuid(const uint8_t *uuid, int uuid_len, 
    bool reconnect)
{
	int retry;
	if (client != NULL) {
        if (reconnect) {
            obexftp_disconnect(client);
            obexftp_close(client);
            sleep(3);
        }
        else
            return TRUE;
    }
    /* Open */
    client = obexftp_open (transport, NULL, info_cb, this);
    if(client == NULL) {
        errBox("Error opening obexftp-client");
        return FALSE;
    }
    if (!use_conn)
        client->quirks &= ~OBEXFTP_CONN_HEADER;
    if (!use_path)
        client->quirks &= ~OBEXFTP_SPLIT_SETPATH;
    for (retry = 0; retry < nRetries; retry++) {
        /* Connect */
        switch (transport) {
        case OBEX_TRANS_IRDA:
            if (obexftp_connect_uuid(client, NULL, 0, uuid, uuid_len) >= 0)
                return TRUE;
            break;
        case OBEX_TRANS_BLUETOOTH:
            if (obexftp_connect_uuid(client, m_device, m_port, 
                uuid, uuid_len) >= 0)
                return TRUE;
            break;
        default:
            errBox("Transport type unknown");
            return FALSE;
        }
        log(tr("Still trying to connect"));
    }
    client = NULL;

	return FALSE;
}

/*
 * Put a message to the log window
 */
void ObexFtpDialog::log(QString str)
{
    browseLog->append(str);
}

void ObexFtpDialog::log(QCString str)
{
    browseLog->append(str);
}

void ObexFtpDialog::log(QString& str)
{
    browseLog->append(str);
}

void ObexFtpDialog::log(const char* str)
{
    browseLog->append(str);
}

void ObexFtpDialog::incProgress()
{
    if (progressStatus < fileProgress->totalSteps())
        fileProgress->setProgress(progressStatus++);
}

void ObexFtpDialog::doneProgress()
{
    progressStatus = 0;
    fileProgress->reset();
}

void ObexFtpDialog::updateDir(const QString& newdir)
{
    localCurdir = newdir;
}

/**
 * Save Log to the specified file
 */
void ObexFtpDialog::slotSaveLog()
{
    QFile logFile(saveLogEdit->text());
    if (!logFile.open(IO_WriteOnly)) {
        errBox(tr("Unable to open file ") + saveLogEdit->text() + tr(" ") + 
            tr(strerror(errno)));
        return;
    }
    QTextStream stream(&logFile);
    stream << browseLog->text() << endl;
    QMessageBox::information(this, tr("Saving"), 
        tr("Log file saved to ") + saveLogEdit->text());
}

void ObexFtpDialog::slotBrowseLog()
{
    MimeTypes types;
    QStringList all;
    all << "*/*";
    types.insert("All Files", all );

    QString str = OFileDialog::getOpenFileName( 1,"/","", types, 0 );
    saveLogEdit->setText(str);
}

/*
 * Event callback function
 */
static void info_cb(int event, const char *msg, int len, void* data)
{
    ObexFtpDialog* dlg = (ObexFtpDialog*)data;
    QCString cmsg(msg, len); //Message to display

	switch (event) {

	case OBEXFTP_EV_ERRMSG:
		dlg->log(QCString("Error: ") + cmsg);
		break;

	case OBEXFTP_EV_ERR:
		dlg->log(QCString("failed: ") + cmsg);
        dlg->doneProgress();
		break;
	case OBEXFTP_EV_OK:
		dlg->log(QCString("done"));
        dlg->doneProgress();
		break;

	case OBEXFTP_EV_CONNECTING:
		dlg->log(QCString("Connecting..."));
		break;
	case OBEXFTP_EV_DISCONNECTING:
		dlg->log(QCString("Disconnecting..."));
		break;
	case OBEXFTP_EV_SENDING:
		dlg->log(QCString("Sending ") + msg);
		break;
	case OBEXFTP_EV_RECEIVING:
		dlg->log(QCString("Receiving ") + msg);
		break;

	case OBEXFTP_EV_LISTENING:
		dlg->log(QCString("Waiting for incoming connection"));
		break;

	case OBEXFTP_EV_CONNECTIND:
		dlg->log(QCString("Incoming connection"));
		break;
	case OBEXFTP_EV_DISCONNECTIND:
		dlg->log(QCString("Disconnecting"));
		break;

	case OBEXFTP_EV_INFO:
        // 64 bit problems ?
		dlg->log(QString("Got info ") + QString::number((int)msg)); 
		break;

	case OBEXFTP_EV_BODY:
		break;

	case OBEXFTP_EV_PROGRESS:
        dlg->incProgress();
		break;
	}
}

//eof
