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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/config.h>

#include <qlineedit.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlabel.h>
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
#include <qmessagebox.h>


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

static const char *SCAP_hostname = "www.handhelds.org";
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

	scapButton = new QPushButton( tr( "Scap" ), this, "ScapButton" );
	scapButton ->setFocusPolicy( QWidget::TabFocus );
	hbox-> addWidget ( scapButton );

	setFixedSize ( sizeHint ( ));
	setFocusPolicy ( QWidget::NoFocus );


	grabTimer = new QTimer ( this, "grab timer");
	
	connect ( grabTimer, SIGNAL( timeout ( )), this, SLOT( performGrab ( )));
	connect ( grabItButton, SIGNAL( clicked ( )), SLOT( slotGrab ( )));
	connect ( scapButton, SIGNAL( clicked ( )), SLOT( slotScap ( )));
}

void ScreenshotControl::slotGrab()
{
	buttonPushed = 1;
	hide();

	setFileName = FALSE;
	if ( saveNamedCheck->isChecked()) {
		setFileName = TRUE;
		InputDialog *fileDlg;

		fileDlg = new InputDialog( 0 , tr("Name of screenshot "), TRUE, 0);
		fileDlg->exec();
		fileDlg->raise();
		QString fileName, list;
		if ( fileDlg->result() == 1 ) {
			fileName = fileDlg->LineEdit1->text();

			if (fileName.find("/", 0, TRUE) == -1)
				FileNamePath = QDir::homeDirPath() + "/Documents/image/png/" + fileName;
			else
				FileNamePath = fileName;

		}
		delete fileDlg;
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
		qDebug("saving file " + fileName);
		snapshot.save( fileName, "PNG");
		QFileInfo fi( fileName);
		lnk.setName( fi.fileName()); //sets file name

		if (!lnk.writeLink())
			qDebug("Writing doclink did not work");
	}
	else {

		fileName = "sc_" + TimeString::dateString( QDateTime::currentDateTime(), false, true);
		fileName.replace(QRegExp("'"), "");
		fileName.replace(QRegExp(" "), "_");
		fileName.replace(QRegExp(":"), ".");
		fileName.replace(QRegExp(","), "");
		QString dirName = QDir::homeDirPath() + "/Documents/image/png/";

		if ( !QDir( dirName).exists() ) {
			qDebug("making dir " + dirName);
			QString msg = "mkdir -p " + dirName;
			system(msg.latin1());
		}
		fileName = dirName + fileName;
		if (fileName.right(3) != "png")
			fileName = fileName + ".png";
		lnk.setFile(fileName); //sets File property
		qDebug("saving file " + fileName);
		snapshot.save( fileName, "PNG");
		QFileInfo fi( fileName);
		lnk.setName( fi.fileName()); //sets file name

		if (!lnk.writeLink())
			qDebug("Writing doclink did not work");

	}

	QPEApplication::beep();
}

void ScreenshotControl::performGrab()
{
	snapshot = QPixmap::grabWindow( QPEApplication::desktop()->winId(), 0, 0, QApplication::desktop()->width(), QApplication::desktop()->height() );

	if (buttonPushed == 1) {
		qDebug("grabbing screen");
		grabTimer->stop();
		show();
		qApp->processEvents();
		savePixmap();
	}
	else {
		grabTimer->stop();

		struct sockaddr_in raddr;
		struct hostent *rhost_info;
		int sock = -1;
		bool ok = false;

		if (( rhost_info = (struct hostent *) ::gethostbyname ((char *) SCAP_hostname )) != 0 ) {
			::memset ( &raddr, 0, sizeof (struct sockaddr_in));
			::memcpy ( &raddr. sin_addr, rhost_info-> h_addr, rhost_info-> h_length );
			raddr. sin_family = rhost_info-> h_addrtype;
			raddr. sin_port = htons ( SCAP_port );

			if (( sock = ::socket ( AF_INET, SOCK_STREAM, 0 )) >= 0 )
			{
				if ( ::connect ( sock, (struct sockaddr *) & raddr, sizeof (struct sockaddr)) >= 0 ) {
					QString header;

					header = "POST /scap/capture.cgi?%1+%2 HTTP/1.1\n"  // 1: model / 2: user
					         "Content-length: 153600\n"
					         "Content-Type: image/gif\n"
					         "Host: %4\n"                               // 3: scap host
					         "\n";

					header = header. arg ( "" ). arg ( ::getenv ( "USER" )). arg ( SCAP_hostname );

					QPixmap pix;

					if ( snapshot. width ( ) == 320 && snapshot. height ( ) == 240 )
					{
						pix = snapshot;
					}
					else if ( snapshot. width ( ) == 240 && snapshot. height ( ) == 320 )
					{
						pix = snapshot. xForm ( QWMatrix ( ). rotate ( 90 ));
					}

					if ( !pix. isNull ( ))
					{
						const char *ascii = header. latin1 ( );
						uint ascii_len = ::strlen ( ascii );

						::write ( sock, ascii, ascii_len );

						QImage img = pix. convertToImage ( ). convertDepth ( 16 );
						::write ( sock, img. bits ( ), img.numBytes ( ));

						ok = true;
					}
				}
				::close ( sock );
			}
		}
		if ( ok )
			QMessageBox::information ( 0, tr( "Success" ), QString ( "<p>%1</p>" ). arg ( tr( "Screenshot was uploaded to %1" )). arg ( SCAP_hostname ));
		else
			QMessageBox::warning ( 0, tr( "Error" ), QString ( "<p>%1</p>" ). arg ( tr( "Connection to %1 failed." )). arg ( SCAP_hostname ));
	}

}



//===========================================================================

ScreenshotApplet::ScreenshotApplet( QWidget *parent, const char *name )
		: QWidget( parent, name )
{
	setFixedHeight( 18 );
	setFixedWidth( 14 );
	
	m_icon = QPixmap ((const char **) snapshot_xpm );
}

ScreenshotApplet::~ScreenshotApplet()
{
}

void ScreenshotApplet::mousePressEvent( QMouseEvent *)
{
	ScreenshotControl *sc = new ScreenshotControl ( );
	QPoint curPos = mapToGlobal ( QPoint ( 0, 0 ));
	sc-> move ( curPos. x ( ) - ( sc-> sizeHint ( ). width ( ) - width ( )) / 2, 
	            curPos. y ( ) - sc-> sizeHint ( ). height ( ));
	sc-> show ( );
}

void ScreenshotApplet::paintEvent( QPaintEvent* )
{
	QPainter p ( this );
	p. drawPixmap ( 0, 1, m_icon );
}

