/**********************************************************************
** Copyright (C) 2002 L.J. Potter ljp@llornkcor.com
**  All rights reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "screenshot.h"
#include "inputDialog.h"

#include <qapplication.h>
#include <stdlib.h>


#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qsocket.h>

#include <qlineedit.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qregexp.h>
#include <qstring.h>
#include <qfile.h>
#include <qtimer.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qcheckbox.h>


static char * snapshot_xpm[] = {
"16 16 10 1",
"   c None",
".  c #000000",
"+  c #00C000",
"@  c #585858",
"#  c #808080",
"$  c #00FF00",
"%  c #008000",
"&  c #00FFFF",
"*  c #FF0000",
"=  c #FFC0C0",
"                ",
"   ...          ",
" ..+++..@#.     ",
" .$++++++.#.    ",
" .%$$++++++.    ",
" .&%%$$++@***.  ",
" .$&$++$=**@+.  ",
" .+$$+++@*$%%.  ",
" .+++++%+++%%.  ",
" .%%++++..+%%.  ",
"  ..%%+++++%%.  ",
"    ..%%+++%%.  ",
"      ..%%+%%.  ",
"        ..%%..  ",
"          ..    ",
"                "};


ScreenshotControl::ScreenshotControl( QWidget *parent, const char *name )
    : QFrame( parent, name, WDestructiveClose | WStyle_StaysOnTop | WType_Popup )
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
//    qDebug("screenshot control");
    QVBoxLayout *vbox = new QVBoxLayout( this );
    QHBoxLayout *hbox = new QHBoxLayout( this );
//    qDebug("new layout");
    delaySpin = new QSpinBox( 0,60,1, this, "Spinner" );
//    qDebug("new spinbox");
    delaySpin->setFocusPolicy( QWidget::NoFocus );


    grabItButton= new QPushButton( this, "GrabButton" );
//    qDebug("new pushbutton");

    grabItButton ->setFocusPolicy( QWidget::TabFocus );
    grabItButton->setText(tr("Snapshot"));


    vbox->setMargin( 6 );
    vbox->setSpacing( 3 );

    vbox->addWidget( delaySpin);

    vbox->setMargin( 6 );
    vbox->setSpacing( 3 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vbox->addItem( spacer );

    saveNamedCheck= new QCheckBox ( "name it", this);
    saveNamedCheck-> setFocusPolicy ( QWidget::NoFocus );
    vbox->addWidget( saveNamedCheck);


    scapButton = new QPushButton( this, "ScapButton" );

    scapButton ->setFocusPolicy( QWidget::TabFocus );
    scapButton ->setText(tr("scap"));
    if( !QFile("/usr/bin/nc").exists())
        scapButton->hide();
        
    hbox->addWidget( grabItButton);
    QSpacerItem* spacer2 = new QSpacerItem( 4, 4, QSizePolicy::Minimum, QSizePolicy::Expanding );
    hbox->addItem( spacer2 );
    hbox->addWidget( scapButton);

    vbox->addItem(hbox);

    setFixedHeight( 100 );
    setFixedWidth( sizeHint().width() );
    setFocusPolicy(QWidget::NoFocus);

    grabTimer= new QTimer(this,"grab timer");
    connect( grabTimer, SIGNAL( timeout() ), this, SLOT(  grabTimerDone() ) );

  delaySpin->setValue(1);
    connect( grabItButton, SIGNAL(released()), SLOT(slotGrab()) );
    connect( scapButton, SIGNAL(released()), SLOT(slotScap()) );
    connect(saveNamedCheck, SIGNAL(toggled( bool)), this, SLOT( nameScreenshot(bool)) );
}

void ScreenshotControl::slotGrab() {
//    qDebug("SlotGrab");
    buttonPushed=1;
    hide();
    setFileName=FALSE;
    if( saveNamedCheck->isChecked()) {
        setFileName=TRUE;
//        qDebug("checked");
        InputDialog *fileDlg;

        fileDlg = new InputDialog( 0 ,tr("Name of screenshot "),TRUE, 0);
        fileDlg->exec();
        fileDlg->raise();
        QString fileName,list;
        if( fileDlg->result() == 1 ) {
            fileName = fileDlg->LineEdit1->text();

            if(fileName.find("/",0,TRUE)==-1) 
                FileNamePath = QDir::homeDirPath()+"/Documents/image/png/"+fileName;
//            qDebug(fileName);
            
        }
        delete fileDlg;
    }
    if ( delaySpin->value() ) {
        grabTimer->start( delaySpin->value() * 1000, true );
    } else {
        show();
    }
}

void ScreenshotControl::slotScap() {
    buttonPushed=2;
    hide();
    if ( delaySpin->value() ) {
        grabTimer->start( delaySpin->value() * 1000, true );
    } else {
        show();
    }
}


void ScreenshotControl::grabTimerDone() {
    performGrab();
}

void ScreenshotControl::savePixmap() {
    DocLnk lnk;
    QString fileName;

    if( setFileName) {
        fileName=FileNamePath;
//not sure why this is needed here, but it forgets fileName
          // if this is below the braces
        
        if(fileName.right(3)!="png")
            fileName=fileName+".png";
        lnk.setFile(fileName); //sets File property
        qDebug("saving file "+fileName);
        snapshot.save( fileName,"PNG");
        QFileInfo fi( fileName);
        lnk.setName( fi.fileName()); //sets file name

        if(!lnk.writeLink())
            qDebug("Writing doclink did not work");
    } else {

        fileName = "sc_"+TimeString::dateString( QDateTime::currentDateTime(),false,true);
        fileName.replace(QRegExp("'"),"");  fileName.replace(QRegExp(" "),"_");  fileName.replace(QRegExp(":"),".");  fileName.replace(QRegExp(","),"");
        QString dirName = QDir::homeDirPath()+"/Documents/image/png/";

        if( !QDir( dirName).exists() ) {
            qDebug("making dir "+dirName);
            QString  msg = "mkdir -p "+dirName;
            system(msg.latin1());
        }
            fileName=dirName+fileName;
            if(fileName.right(3)!="png")
                fileName=fileName+".png";
            lnk.setFile(fileName); //sets File property
            qDebug("saving file "+fileName);
            snapshot.save( fileName,"PNG");
            QFileInfo fi( fileName);
            lnk.setName( fi.fileName()); //sets file name

            if(!lnk.writeLink())
                qDebug("Writing doclink did not work");

    }

    QPEApplication::beep();
}

void ScreenshotControl::performGrab() {

    if(buttonPushed ==1) {
        qDebug("grabbing screen");
        grabTimer->stop();
        snapshot = QPixmap::grabWindow( QPEApplication::desktop()->winId(),0,0,QApplication::desktop()->width(),QApplication::desktop()->height() );
        show();
        qApp->processEvents();        
        savePixmap();
    } else {
        qDebug("scap");
        grabTimer->stop();
// do scap here
        QString cmd;
        cmd="cat /dev/fd0 > /tmp/cap";
        system(cmd.latin1());
// qDebug("echo \"POST /scap/capture.cgi http1.1\"
// echo \"Content-length: 153600\"
// echo \"Content-Type: image/gif\"
// echo \"HOST: www.handhelds.org\"
// echo \"\"
// cat /tmp/cap | nc h1.handhelds.org 80");
        cmd="nc h1.handhelds.org 1011 </tmp/cap";
        qDebug("running command "+cmd);

        system(cmd.latin1());
        show();
    }
    
}

void ScreenshotControl::setTime(int newTime) {
       delaySpin->setValue(newTime);
}

void ScreenshotControl::nameScreenshot(bool b) {

    
}

//===========================================================================

ScreenshotApplet::ScreenshotApplet( QWidget *parent, const char *name )
    : QWidget( parent, name ) {
//    qDebug("beginning  applet");
    setFixedHeight( 18 );
    setFixedWidth( 14 );
    vc = new ScreenshotControl;
//    qDebug("new screenshotapplet");
}

ScreenshotApplet::~ScreenshotApplet() {
}

void ScreenshotApplet::mousePressEvent( QMouseEvent *) {
//    if(!vc)
    vc = new ScreenshotControl;
    QPoint curPos = mapToGlobal( rect().topLeft() );
    vc->move( curPos.x()-(vc->sizeHint().width()-width())/2, curPos.y() - 100 );
    vc->show();

}

void ScreenshotApplet::paintEvent( QPaintEvent* ) {
    QPainter p(this);
    qDebug("paint pixmap");
    p.drawPixmap( 0, 1,  ( const char** ) snapshot_xpm );

}

