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

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/ofiledialog.h>
#include <opie2/oresource.h>
#include <opie2/otaskbarapplet.h>

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>

/* QT */
#include <qlineedit.h>
#include <qdir.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qmessagebox.h>

/* STD */
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

using namespace Opie::Core;
using namespace Opie::Ui;

static const char *SCAP_hostname = "www.handhelds.org";
static const char *SCAP_capture_path = "/scap/capture.cgi";
static const char *SCAP_view_url = "http://www.handhelds.org/scap";
static const int SCAP_port = 80;


ScreenshotControl::ScreenshotControl( QWidget *parent, const char *name )
    : QFrame( parent, name, WDestructiveClose | WStyle_StaysOnTop | WType_Popup )
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    QVBoxLayout *vbox = new QVBoxLayout ( this, 5, 3 );
    QHBoxLayout *hbox;

    hbox = new QHBoxLayout ( vbox );

    QLabel *l = new QLabel ( tr( "Delay" ), this );
    hbox-> addWidget ( l );

    delaySpin = new QSpinBox( 0, 60, 1, this, "Spinner" );
    delaySpin-> setButtonSymbols ( QSpinBox::PlusMinus );
    delaySpin-> setSuffix ( tr( "sec" ));
    delaySpin-> setFocusPolicy( QWidget::NoFocus );
    delaySpin-> setValue ( 1 );
    hbox-> addWidget ( delaySpin );

    saveNamedCheck = new QCheckBox ( tr( "Save named" ), this);
    saveNamedCheck-> setFocusPolicy ( QWidget::NoFocus );
    vbox->addWidget( saveNamedCheck);

    vbox-> addSpacing ( 3 );

    l = new QLabel ( tr( "Save screenshot as..." ), this );
    vbox-> addWidget ( l, AlignCenter );

    hbox = new QHBoxLayout ( vbox );

    grabItButton = new QPushButton( tr( "File" ), this, "GrabButton" );
    grabItButton ->setFocusPolicy( QWidget::TabFocus );
    hbox-> addWidget ( grabItButton );

    QPushButton* drawPadButton = new QPushButton( tr("Opie drawpad"), this, "DrawPadButton" );
    drawPadButton->setFocusPolicy( QWidget::TabFocus );
    hbox->addWidget( drawPadButton );

    scapButton = new QPushButton( tr( "Scap" ), this, "ScapButton" );
    scapButton ->setFocusPolicy( QWidget::TabFocus );
    hbox-> addWidget ( scapButton );

    setFixedSize ( sizeHint ( ));
    setFocusPolicy ( QWidget::NoFocus );


    grabTimer = new QTimer ( this, "grab timer");

    connect ( grabTimer, SIGNAL( timeout()), this, SLOT( performGrab()));
    connect ( grabItButton, SIGNAL( clicked()), SLOT( slotGrab()));
    connect ( scapButton, SIGNAL( clicked()), SLOT( slotScap()));
    connect ( drawPadButton, SIGNAL(clicked()), SLOT(slotDrawpad()) );
}

void ScreenshotControl::slotGrab()
{
    buttonPushed = 1;
    hide();

    setFileName = FALSE;
    if ( saveNamedCheck->isChecked()) {
        setFileName = TRUE;

        MimeTypes types;
        QStringList list;
        list << "image/*";
        types. insert ( "Images", list );

        FileNamePath = Opie::Ui::OFileDialog::getSaveFileName( 1,"/","", types, 0 );
    }

    if ( delaySpin->value() )
        grabTimer->start( delaySpin->value() * 1000, true );
    else
        show();
}

void ScreenshotControl::slotScap()
{
    buttonPushed = 2;
    hide();

    if ( delaySpin->value() )
        grabTimer->start( delaySpin->value() * 1000, true );
    else
        show();
}

void ScreenshotControl::slotDrawpad()
{
    buttonPushed = 3;
    hide();
    if ( delaySpin->value() )
        grabTimer->start( delaySpin->value()*1000, true );
    else
        show();
}

void ScreenshotControl::savePixmap()
{
    DocLnk lnk;
    QString fileName;

    if ( setFileName) {
        fileName = FileNamePath;
        //not sure why this is needed here, but it forgets fileName
        // if this is below the braces

        if (fileName.right(3) != "png")
            fileName = fileName + ".png";
        lnk.setFile(fileName); //sets File property
        odebug << "saving file " + fileName << oendl;
        snapshot.save( fileName, "PNG");
        QFileInfo fi( fileName);
        lnk.setName( fi.fileName()); //sets file name

        if (!lnk.writeLink())
            odebug << "Writing doclink did not work" << oendl;
    }
    else {

        fileName = "sc_" + QDateTime::currentDateTime().toString();
        fileName.replace(QRegExp("'"), "");
        fileName.replace(QRegExp(" "), "_");
        fileName.replace(QRegExp(":"), ".");
        fileName.replace(QRegExp(","), "");
        QString dirName = QDir::homeDirPath() + "/Documents/image/png/";

        if ( !QDir( dirName).exists() ) {
            odebug << "making dir " + dirName << oendl;
            QString msg = "mkdir -p " + dirName;
            system(msg.latin1());
        }
        fileName = dirName + fileName;
        if (fileName.right(3) != "png")
            fileName = fileName + ".png";
        lnk.setFile(fileName); //sets File property
        odebug << "saving file " + fileName << oendl;
        snapshot.save( fileName, "PNG");
        QFileInfo fi( fileName);
        lnk.setName( fi.fileName()); //sets file name

        if (!lnk.writeLink())
            odebug << "Writing doclink did not work" << oendl;
    }

    QPEApplication::beep();
}

void ScreenshotControl::performGrab()
{
    snapshot = QPixmap::grabWindow( QPEApplication::desktop()->winId(), 0, 0, QApplication::desktop()->width(),  QApplication::desktop()->height() );

    if (buttonPushed == 1) {
        odebug << "grabbing screen" << oendl;
        grabTimer->stop();
        show();
        qApp->processEvents();
        savePixmap();
    }
    else if ( buttonPushed == 3 ) {
        grabTimer->stop();
        show();
        QCopEnvelope env("QPE/Application/drawpad", "importPixmap(QPixmap)" );
        env << snapshot;
    }
    else {
        grabTimer->stop();

        struct sockaddr_in raddr;
        struct hostent *rhost_info;
        int sock = -1;
        bool ok = false;

        QString displayEnv = getenv("QWS_DISPLAY");
        odebug << displayEnv << oendl;

        if(( displayEnv.left(2) != ":0" ) && (!displayEnv.isEmpty())) {

            if (( rhost_info = (struct hostent *) ::gethostbyname ((char *) SCAP_hostname )) != 0 ) {
                ::memset ( &raddr, 0, sizeof (struct sockaddr_in));
                ::memcpy ( &raddr. sin_addr, rhost_info-> h_addr, rhost_info-> h_length );
                raddr. sin_family = rhost_info-> h_addrtype;
                raddr. sin_port = htons ( SCAP_port );

                if (( sock = ::socket ( AF_INET, SOCK_STREAM, 0 )) >= 0 ) {
                    if ( ::connect ( sock, (struct sockaddr *) & raddr, sizeof (struct sockaddr)) >= 0 ) {
                        QString header;

                        QPixmap pix;

                        QString SCAP_model = ODevice::inst()->modelString();
                        SCAP_model.replace(QRegExp(" "), "%%%");

                        if( displayEnv == "QVFb:0" )  {//set this if you plan on using this app in qvfb!!
                            pix = snapshot.xForm(QWMatrix().rotate(90));
                        }
                        else
                            pix = ( snapshot.width() > snapshot.height() ) ? snapshot : snapshot.xForm( QWMatrix().rotate(90) );

                        QImage img = pix.convertToImage().convertDepth( 16 ); // could make that also depth independent, if hh.org/scap can handle it

                        header = "POST %1?%2+%3+%4 HTTP/1.1\n"  // 1: path / 2: model / 3: user / 4: resolution
                                "Content-length: %5\n"                     // 5: content length
                                "Content-Type: image/png\n"
                                "Host: %6\n"                               // 6: scap host
                                "\n";

                        QString imgres = QString("%1x%2").arg( QApplication::desktop()->width() ).arg( QApplication::desktop()->height() );

                        header = header.arg( SCAP_capture_path ).arg( SCAP_model).arg( ::getenv( "USER" ) ).arg( imgres ).arg( img.numBytes() ).arg( SCAP_hostname );
                        header.replace(QRegExp("%%%"), "%20");
                        odebug << header << oendl;

                        if ( !pix.isNull() )  {
                            const char *ascii = header.latin1( );
                            uint ascii_len = ::strlen( ascii );
                            ::write ( sock, ascii, ascii_len );
                            ::write ( sock, img.bits(), img.numBytes() );

                            ok = true;
                        }
                    }
                    ::close ( sock );
                }
            }
            if ( ok ) {
                QMessageBox::information( 0, tr( "Success" ), QString( "<p>%1</p>" ).arg ( tr( "Screenshot was uploaded to %1" )).arg( SCAP_view_url ));
            }
            else {
                QMessageBox::warning( 0, tr( "Error" ), QString( "<p>%1</p>" ).arg( tr( "Connection to %1 failed." )).arg( SCAP_hostname ));
            }
        }
        else {
            QMessageBox::warning( 0, tr( "Error" ),tr("Please set <b>QWS_DISPLAY</b> environmental variable."));
        }
    }
}


//===========================================================================

ScreenshotApplet::ScreenshotApplet( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );
    m_icon = Opie::Core::OResource::loadPixmap( "screenshotapplet/screenshot", Opie::Core::OResource::SmallIcon );
}

ScreenshotApplet::~ScreenshotApplet()
{
}

int ScreenshotApplet::position()
{
    return 6;
}

void ScreenshotApplet::mousePressEvent( QMouseEvent *)
{
    ScreenshotControl *sc = new ScreenshotControl ( );
    QPoint curPos = mapToGlobal ( QPoint ( 0, 0 ));

    // windowPosX is the windows position centered above the applets icon.
    // If the icon is near the edge of the screen, the window would leave the visible area
    // so we check the position against the screen width and correct the difference if needed

    int screenWidth = qApp->desktop()->width();
    int windowPosX = curPos. x ( ) - ( sc-> sizeHint ( ). width ( ) - width ( )) / 2 ;
    int ZwindowPosX, XwindowPosX;

    // the window would be placed beyond the screen wich doesn't look tooo good
    if ( (windowPosX + sc-> sizeHint ( ). width ( )) > screenWidth ) {
        XwindowPosX = windowPosX + sc-> sizeHint ( ). width ( ) - screenWidth;
        ZwindowPosX = windowPosX - XwindowPosX - 1;
    }
    else {
        ZwindowPosX = windowPosX;
    }

    sc-> move ( ZwindowPosX, curPos. y ( ) - sc-> sizeHint ( ). height ( ) );
    sc-> show ( );
}

void ScreenshotApplet::paintEvent( QPaintEvent* )
{
    QPainter p ( this );
    p.drawPixmap( 0, 2, m_icon );
}


EXPORT_OPIE_APPLET_v1( ScreenshotApplet )

