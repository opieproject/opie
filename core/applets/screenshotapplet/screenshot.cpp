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
#include <qbuffer.h>

/* STD */
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

using namespace Opie::Core;
using namespace Opie::Ui;

static const char *SCAP_hostname = "scap.linuxtogo.org";
static const char *SCAP_capture_path = "/tickle.php";
static const char *SCAP_view_url = "http://scap.linuxtogo.org";
static const int SCAP_port = 80;

#define SEP "----------------------------092f83a3d9e9"


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
                        QString header, imgheader, footer;

                        QString SCAP_model = ODevice::inst()->modelString();
                        SCAP_model.replace(QRegExp(" "), "%%%");

                        QImage img = snapshot.convertToImage();
                        QBuffer buf;
                        buf.open(IO_WriteOnly );
                        QImageIO imageio( &buf, "PNG" );
                        imageio.setImage( img );
                        imageio.write();
                        uint bufsize = buf.buffer().size();

                        header = "POST %1 HTTP/1.1\r\n"  // 1: path
                                 "User-Agent: opie-screenshotapplet/1.0\r\n"
                                 "Host: %2\r\n"                               // 2: scap host
                                 "Accept: */*\r\n"
                                 "Content-Length: %3\r\n"                     // 3: content length
                                 "Content-Type: multipart/form-data; boundary=" SEP "\r\n"
                                 "\r\n";

                        imgheader = "--" SEP "\r\n"
                                 "Content-Disposition: form-data; name=\"file\"; filename=\"scap%1.png\"\r\n"
                                 "Content-Type: image/png\r\n"
                                 "\r\n";

                        int id = rand() / (RAND_MAX / 65536);
                        imgheader = imgheader.arg(id);

                        footer = "\r\n"
                                 "--" SEP "\r\n"
                                 "Content-Disposition: form-data; name=\"key\"\r\n"
                                 "\r\n"
                                 "secret\r\n"
                                 "--" SEP "\r\n"
                                 "Content-Disposition: form-data; name=\"model\"\r\n"
                                 "\r\n"
                                 "%1\r\n"
                                 "--" SEP "\r\n"
                                 "Content-Disposition: form-data; name=\"submit\"\r\n"
                                 "\r\n"
                                 "Upload\r\n"
                                 "--" SEP "\r\n"
                                 "Content-Disposition: form-data; name=\"text\"\r\n"
                                 "\r\n"
                                 "No comment\r\n"
                                 "--" SEP "--\r\n";

                        footer = footer.arg( SCAP_model );
                        footer.replace(QRegExp("%%%"), "%20");

                        // We do this last because we want to set the content length based on the rest of the message
                        header = header.arg( SCAP_capture_path ).arg( SCAP_hostname ).arg( bufsize + imgheader.length() + footer.length() );
                        odebug << header << oendl;
                        odebug << imgheader << oendl;

                        if ( bufsize > 200 )  {
                            // Send header
                            const char *ascii = header.latin1();
                            uint ascii_len = ::strlen( ascii );
                            ::write ( sock, ascii, ascii_len );
                            // Send image
                            ascii = imgheader.latin1();
                            ascii_len = ::strlen( ascii );
                            ::write ( sock, ascii, ascii_len );
                            ::write ( sock, buf.buffer().data(), bufsize );
                            // Send the remainder of the message
                            ascii = footer.latin1();
                            ascii_len = ::strlen( ascii );
                            ::write ( sock, ascii, ascii_len );
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

