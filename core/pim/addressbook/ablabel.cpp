/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "ablabel.h"

#include <qpe/stringutil.h>

#include <qregexp.h>
#include <qstylesheet.h>

AbLabel::AbLabel( QWidget *parent, const char *name ):
	QTextView( parent, name ),
	m_empty( false )
{
}

AbLabel::~AbLabel()
{
}

void AbLabel::setContacts( const OContactAccess::List& viewList )
{
	m_viewList = viewList;
	if (m_viewList.count() != 0){
		m_empty = false;
		m_itCurContact = m_viewList.begin();
		sync();
	}else{
		// m_itCurContact.clear();
		m_empty = true;
		setText( "" );
	}
}

int AbLabel::currentEntry_UID()
{
	OContact contact = currentEntry();

	if ( contact.isEmpty() )
		return 0;
	else
		return ( contact.uid() );
}

OContact AbLabel::currentEntry()
{
	if ( ! m_empty )
		return ( *m_itCurContact );
	else
		return OContact();
}


bool AbLabel::selectContact( int UID )
{

	for ( int r = 0; r < m_viewList.count(); ++r ) {
		if ( m_viewList.uidAt( r ) == UID ){
			m_itCurContact.setCurrent( r );
			break;
		}
	}

	sync();

	return true;
}



void AbLabel::sync()
{
    QString text = (*m_itCurContact).toRichText();
    setText( text );
}

void AbLabel::keyPressEvent( QKeyEvent *e )
{

	// Commonly handled keys
	if ( !m_empty ){
		switch( e->key() ) {
		case Qt::Key_Left:
			qWarning( "Left..");
		case Qt::Key_Right:
			qWarning( "Right..");
		case Qt::Key_F33:
			qWarning( "OK..");
			emit signalOkPressed();
			break;
		case Qt::Key_Up:
			qWarning( "UP..");
			if ( ( visibleHeight() < contentsHeight() ) &&
			     ( verticalScrollBar()->value() > verticalScrollBar()->minValue() ) )
				scrollBy( 0, -(visibleHeight()-20) );
			else {
				--m_itCurContact;
				if ( *m_itCurContact != OContact() )
					sync();
				else
					m_itCurContact = m_viewList.end();
			}

			break;
		case Qt::Key_Down:
			qWarning( "DOWN..");
// 			qWarning( "visible: %d, content: %d",visibleHeight(),contentsHeight()); 
// 			qWarning( "value: %d; barMaxValue: %d", verticalScrollBar()->value()
// 				  , verticalScrollBar()->maxValue() );
			if ( ( visibleHeight() < contentsHeight() ) &&
			     ( verticalScrollBar()->value() < verticalScrollBar()->maxValue() ) )
				scrollBy( 0, visibleHeight()-20 );
			else {
				++m_itCurContact;
				if ( *m_itCurContact != OContact() )
					sync();
				else
					m_itCurContact = m_viewList.begin();
			}
			break;
                case Qt::Key_Return: // fall through
                case Qt::Key_Space: // fall through
                case Qt::Key_Enter: // we want to switch back
                        emit signalOkPressed();
                        break;
                default: break;
		}
	}

}
