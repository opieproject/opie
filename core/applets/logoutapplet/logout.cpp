#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qapplication.h>
#include <qiconset.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include <unistd.h>

#include "logout.h"


LogoutApplet::LogoutApplet ( )
	: QObject ( 0, "LogoutApplet" ), ref ( 0 )
{
}

LogoutApplet::~LogoutApplet ( )
{
}

int LogoutApplet::position ( ) const
{
    return 0;
}

QString LogoutApplet::name ( ) const
{
	return tr( "Logout shortcut" );
}

QString LogoutApplet::text ( ) const
{
	return tr( "Logout" );
}

QIconSet LogoutApplet::icon ( ) const
{
	QPixmap pix;
	QImage img = Resource::loadImage ( "logout" );
	
	if ( !img. isNull ( ))
		pix. convertFromImage ( img. smoothScale ( 14, 14 ));
	return pix;
}

QPopupMenu *LogoutApplet::popup ( QWidget * ) const
{
	return 0;
}

// This is a workaround for a Qt bug
// clipboard applet has to stop its poll timer, or Qt/E
// will hang on quit() right before it emits aboutToQuit()

class HackApplication : public QApplication {
public:
	HackApplication ( ) : QApplication ( dummy, 0 ) 
	{ 
	}

	void emit_about_to_quit ( )
	{
		emit aboutToQuit ( );		
	}
	
	int dummy;
};


void LogoutApplet::activated ( )
{
	QMessageBox mb ( tr( "Logout" ), 
	                 tr( "Do you really want to\nend this session ?" ), 
	                 QMessageBox::NoIcon, 
	                 QMessageBox::Yes | QMessageBox::Default,
	                 QMessageBox::No | QMessageBox::Escape,
	                 QMessageBox::NoButton );

	mb. setButtonText ( QMessageBox::Yes, "Yes" );
	mb. setButtonText ( QMessageBox::No, "No" );
	mb. setIconPixmap ( icon ( ). pixmap ( ));

	if ( mb. exec ( ) == QMessageBox::Yes ) {
		{ QCopEnvelope envelope( "QPE/System", "forceQuit()" ); }

		qApp-> processEvents ( ); // ensure the message goes out.
		sleep ( 1 );  // You have 1 second to comply.
		
		((HackApplication *) qApp )-> emit_about_to_quit ( );
		qApp-> quit();
	}
}


QRESULT LogoutApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_MenuApplet )
		*iface = this;

	if ( *iface )
		(*iface)-> addRef ( );
	return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
	Q_CREATE_INSTANCE( LogoutApplet )
}


