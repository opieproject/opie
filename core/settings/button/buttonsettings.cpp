/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:       
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qlayout.h>
#include <qlabel.h>
#include <qtimer.h>

#include <qpe/qpeapplication.h>

#include <opie/odevice.h>

#include "buttonsettings.h"
#include "buttonutils.h"
#include "remapdlg.h"

using namespace Opie;

struct buttoninfo {
	const ODeviceButton *m_button;
	int          m_index;

	OQCopMessage m_pmsg;
	QLabel      *m_picon;
	QLabel      *m_plabel;

	OQCopMessage m_hmsg;
	QLabel      *m_hicon;
	QLabel      *m_hlabel;
	
	bool        m_pdirty : 1;
	bool        m_hdirty : 1;
};


ButtonSettings::ButtonSettings ( ) 
	: QDialog ( 0, "ButtonSettings", false, WStyle_ContextHelp )
{
	const QValueList <ODeviceButton> &buttons = ODevice::inst ( )-> buttons ( );
	(void) ButtonUtils::inst ( ); // initialise

	setCaption ( tr( "Button Settings" ));

	QVBoxLayout *toplay = new QVBoxLayout ( this, 3, 3 );
	
	QLabel *l = new QLabel ( tr( "<center>Press or hold the button you want to remap.</center>" ), this );
	toplay-> addWidget ( l );
	
	QGridLayout *lay = new QGridLayout ( toplay );
	lay-> setMargin ( 0 );
	lay-> setColStretch ( 0, 0 );
	lay-> setColStretch ( 1, 0 );
	lay-> setColStretch ( 2, 0 );
	lay-> setColStretch ( 3, 10 );

	m_infos. setAutoDelete ( true );
	
	int i = 1;
	int index = 0;
	for ( QValueList<ODeviceButton>::ConstIterator it = buttons. begin ( ); it != buttons. end ( ); it++ ) {
		if ( it != buttons. begin ( )) {
			QFrame *f = new QFrame ( this );
			f-> setFrameStyle ( QFrame::Sunken | QFrame::VLine );
			lay-> addMultiCellWidget ( f, i, i, 0, 3 );
			i++;
		}

		buttoninfo *bi = new buttoninfo ( );
		bi-> m_button = &(*it);
		bi-> m_index = index++;
		bi-> m_pmsg = (*it). pressedAction ( );
		bi-> m_hmsg = (*it). heldAction ( );
		bi-> m_pdirty = false;
		bi-> m_hdirty = false;
	
		l = new QLabel ( this );
		l-> setPixmap (( *it ). pixmap ( ));
		
		lay-> addMultiCellWidget ( l, i, i + 1, 0, 0 );
		
		l = new QLabel ( tr( "Press:" ), this );
		lay-> addWidget ( l, i, 1, AlignLeft | AlignBottom );
		l = new QLabel ( tr( "Hold:" ), this );
		lay-> addWidget ( l, i + 1, 1, AlignLeft | AlignTop );
					
		l = new QLabel ( this );
		l-> setFixedSize ( 16, 16 );
		lay-> addWidget ( l, i, 2, AlignLeft | AlignBottom );
		bi-> m_picon = l;

		l = new QLabel ( this );
		l-> setAlignment ( AlignLeft | AlignVCenter | SingleLine );
		lay-> addWidget ( l, i, 3, AlignLeft | AlignBottom );
		bi-> m_plabel = l;
		
		l = new QLabel ( this );
		l-> setFixedSize ( 16, 16 );
		lay-> addWidget ( l, i + 1, 2, AlignLeft | AlignTop );
		bi-> m_hicon = l;

		l = new QLabel ( this );
		l-> setAlignment ( AlignLeft | AlignVCenter | SingleLine );
		lay-> addWidget ( l, i + 1, 3, AlignLeft | AlignTop );
		bi-> m_hlabel = l;
		
		i += 2;
		
		m_infos. append ( bi );
	}	

	toplay-> addStretch ( 10 );

	m_last_button = 0;
	m_lock = false;
	
	m_timer = new QTimer ( this );
	connect ( m_timer, SIGNAL( timeout ( )), this, SLOT( keyTimeout ( )));

	updateLabels ( );	
	
	QPEApplication::grabKeyboard ( );
}

ButtonSettings::~ButtonSettings ( )
{
	QPEApplication::ungrabKeyboard ( );
}

void ButtonSettings::updateLabels ( )
{
	for ( QListIterator <buttoninfo> it ( m_infos ); *it; ++it ) {
		qCopInfo cip = ButtonUtils::inst ( )-> messageToInfo ((*it)-> m_pmsg );
		
		(*it)-> m_picon-> setPixmap ( cip. m_icon );
		(*it)-> m_plabel-> setText ( cip. m_name );

		qCopInfo cih = ButtonUtils::inst ( )-> messageToInfo ((*it)-> m_hmsg );

		(*it)-> m_hicon-> setPixmap ( cih. m_icon );
		(*it)-> m_hlabel-> setText ( cih. m_name );
	}
}

buttoninfo *ButtonSettings::buttonInfoForKeycode ( ushort key )
{
	for ( QListIterator <buttoninfo> it ( m_infos ); *it; ++it ) {
		if ((*it)-> m_button-> keycode ( ) == key )
			return *it; 
	}  
	return 0;  	
}

void ButtonSettings::keyPressEvent ( QKeyEvent *e )
{
	buttoninfo *bi = buttonInfoForKeycode ( e-> key ( ));
	
	if ( bi && !e-> isAutoRepeat ( )) {	
		m_timer-> stop ( );
		m_last_button = bi;
		m_timer-> start ( ODevice::inst ( )-> buttonHoldTime ( ), true );
	}
	else
		QDialog::keyPressEvent ( e );
}

void ButtonSettings::keyReleaseEvent ( QKeyEvent *e )
{
	buttoninfo *bi = buttonInfoForKeycode ( e-> key ( ));
	
	if ( bi && !e-> isAutoRepeat ( ) && m_timer-> isActive ( )) {
		m_timer-> stop ( );
		edit ( bi, false );		
	}
	else
		QDialog::keyReleaseEvent ( e );
}

void ButtonSettings::keyTimeout ( )
{
	if ( m_last_button ) {
		edit ( m_last_button, true );
		m_last_button = false;
	}
}

void ButtonSettings::edit ( buttoninfo *bi, bool hold )
{
	qDebug ( "remap %s for %s", hold ? "hold" : "press", bi-> m_button-> userText ( ). latin1 ( )); 	
	
	if ( m_lock )
		return;
	m_lock = true;
	
	RemapDlg *d = new RemapDlg ( bi-> m_button, hold, this );

	d-> showMaximized ( );	
	if ( d-> exec ( ) == QDialog::Accepted ) {
		qDebug ( " -> %s %s", d-> message ( ). channel ( ). data ( ), d-> message ( ). message ( ). data ( ));
	
		if ( hold ) {
			bi-> m_hmsg = d-> message ( );
			bi-> m_hdirty = true;
		}
		else {
			bi-> m_pmsg = d-> message ( );
			bi-> m_pdirty = true;
		}
	
		updateLabels ( );
	}
		
	delete d;
	
	m_lock = false;
}

void ButtonSettings::accept ( )
{
	for ( QListIterator <buttoninfo> it ( m_infos ); *it; ++it ) {
		buttoninfo *bi = *it;
		
		if ( bi-> m_pdirty )
			ODevice::inst ( )-> remapPressedAction ( bi-> m_index, bi-> m_pmsg );
		if ( bi-> m_hdirty )
			ODevice::inst ( )-> remapHeldAction ( bi-> m_index, bi-> m_hmsg );
	}
	QDialog::accept ( );
}

void ButtonSettings::done ( int r )
{
	QDialog::done ( r );
	close ( );
}
