/* $Id: pppdialog.cpp,v 1.12 2006-04-09 17:14:19 korovkin Exp $ */
/* PPP/rfcomm connection dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "pppdialog.h"
#include "rfcommhelper.h"
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qdir.h>
#include <opie2/oprocess.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

using namespace OpieTooth;

using namespace Opie::Core;

Connection PPPDialog::conns[NCONNECTS];

PPPDialog::PPPDialog( const QString& device, int port, QWidget* parent,
    const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
    int i; //Just an index variable
    QDir d("/etc/ppp/peers/"); //Dir we search files in
    d.setFilter( QDir::Files);
    d.setSorting( QDir::Size | QDir::Reversed );

    if ( !name )
        setName( "PPPDialog" );
    setCaption( tr( "ppp connection " ) ) ;

    m_device = device;
    m_port = port;

    layout = new QVBoxLayout( this );

    QLabel* info = new QLabel( this );
    info->setText( tr("Enter a ppp script name:") );

    cmdLine = new QComboBox( this );
    cmdLine->setEditable(true);

    outPut = new QMultiLineEdit( this );
    QFont outPut_font(  outPut->font() );
    outPut_font.setPointSize( 8 );
    outPut->setFont( outPut_font );
    outPut->setWordWrap( QMultiLineEdit::WidgetWidth );

    connectButton = new QPushButton( this );
    connectButton->setText( tr( "Connect" ) );

    serPort = new QComboBox(this);
    for (i = 0; i < NCONNECTS; i++) {
        if (!PPPDialog::conns[i].proc.isRunning())
            serPort->insertItem(QString("rfcomm%1").arg(i));
    }

    layout->addWidget(info);
    layout->addWidget(cmdLine);
    layout->addWidget(serPort);
    layout->addWidget(outPut);
    layout->addWidget(connectButton);

    connect( connectButton, SIGNAL( clicked() ), this,  SLOT( connectToDevice() ) );
    //And fill cmdLine with ppp script filenames
    cmdLine->insertItem("");
    cmdLine->insertStringList(d.entryList());
}

PPPDialog::~PPPDialog() {
}

void PPPDialog::connectToDevice() {
    int portNum = serPort->currentText().right(1).toInt();
    if (PPPDialog::conns[portNum].proc.isRunning()) {
        outPut->append(tr("Work in progress"));
        return;
    }
    outPut->clear();
    PPPDialog::conns[portNum].proc.clearArguments();
    // vom popupmenu beziehen
    if (cmdLine->currentText().isEmpty()) {//Connect by rfcomm
        PPPDialog::conns[portNum].proc << "rfcomm" << "connect"
            << QString::number(portNum) << m_device << QString::number(m_port);
    }
    else {
        PPPDialog::conns[portNum].proc << "pppd"
            << QString("/dev/bluetooth/rfcomm/%1").arg(portNum)
            << "call"
            << cmdLine->currentText();
    }
    if (!PPPDialog::conns[portNum].proc.start(OProcess::NotifyOnExit,
        OProcess::All)) {
        outPut->append(tr("Couldn't start"));
    }
    else
    {
        PPPDialog::conns[portNum].proc.resume();
        outPut->append(tr("Started"));
        PPPDialog::conns[portNum].btAddr = m_device;
        PPPDialog::conns[portNum].port = m_port;
        connect(&PPPDialog::conns[portNum].proc,
		    SIGNAL(receivedStdout(Opie::Core::OProcess*, char*, int)),
            this, SLOT(fillOutPut(Opie::Core::OProcess*, char*, int)));
        connect( &PPPDialog::conns[portNum].proc,
            SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int)),
            this,    SLOT(fillErr(Opie::Core::OProcess*, char*, int)));
        connect( &PPPDialog::conns[portNum].proc,
            SIGNAL(processExited(Opie::Core::OProcess*)),
            this, SLOT(slotProcessExited(Opie::Core::OProcess*)));
    }
}

void PPPDialog::fillOutPut( OProcess*, char* cha, int len ) {
    QCString str(cha, len);
    outPut->append(str);
}

void PPPDialog::fillErr(OProcess*, char* buf, int len)
{
    QCString str(buf, len);
    outPut->append(str);
}

void PPPDialog::slotProcessExited(OProcess* proc) {
    if (proc->normalExit()) {
        outPut->append( tr("Finished with result ") );
        outPut->append( QString::number(proc->exitStatus()) );
    }
    else
        outPut->append( tr("Exited abnormally") );
}

void PPPDialog::closeEvent(QCloseEvent* e)
{
    int i; //index variable
    for (i = 0; i < NCONNECTS; i++) {
        if(PPPDialog::conns[i].proc.isRunning())
            PPPDialog::conns[i].proc.kill();
    }
    QDialog::closeEvent(e);
}

//eof
