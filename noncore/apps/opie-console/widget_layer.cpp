/* ------------------------------------------------------------------------- */
/*                                                                           */
/*  widget_layer.cpp          Widget Layer                                   */
/*                                                                           */
/*              opie developers <opie@handhelds.org>                         */
/*                                                                           */
/* ------------------------------------------------------------------------- */



#include "widget_layer.h"

#include <qclipboard.h>
#include <qregexp.h>
#include <qglobal.h>

#include <string.h>
//#include <

#include "common.h"


#define loc(X,Y) ((Y)*m_columns+(X))


WidgetLayer::WidgetLayer( QObject *parent, const char *name  ) : QObject( parent, name )
{
	// get the clipboard
	m_clipboard = QApplication::clipboard();
	
	// when data on clipboard changes, clear selection
	QObject::connect( (QObject*) m_clipboard, SIGNAL( dataChanged() ),
			(QObject*)this, SLOT( onClearSelection() ) );

	// initialize vars:
	m_lines = 1;
	m_columns = 1;
	m_resizing = false;
	
	m_image = QArray<Character>();
}

WidgetLayer::~WidgetLayer()
{
	// clean up!
}

/* --------------------------------- audio ---------------------------------- */

void WidgetLayer::bell()
{
	QApplication::beep();
}


/* --------------------------------- screen --------------------------------- */


void WidgetLayer::propagateSize()
{
	QArray<Character> oldimage = m_image;
	int oldlines = m_lines;
	int oldcolumns = m_columns;
	
	makeImage();
	
	// copy old image, to reduce flicker
	if ( oldimage )
	{
		//FIXME: is it possible to do this with QArray.dublicate()?
		
		int lins = QMIN( oldlines, m_lines );
		int cols = QMIN( oldcolumns, m_columns );
		for ( int lin = 0; lin < lins; ++lin )
		{
			memcpy( (void*) &m_image[m_columns*lin],
				(void*) &oldimage[oldcolumns*lin],
				cols*sizeof( Character ) );
		}
		//free( oldimage );
	}
	else
		clearImage();
	
	m_resizing = true;
	emit imageSizeChanged( m_lines, m_columns );
	m_resizing = false;
}

void WidgetLayer::makeImage()
{
	calcGeometry();
	m_image = QArray<Character>();
	clearImage(); 
}

void WidgetLayer::clearImage()
{
	//should this belong here??
	for ( int y = 0; y < m_lines; y++ )
	for ( int x = 0; x < m_columns; x++ )
	{
		m_image[loc(x,y)].c = 0xff; 
		m_image[loc(x,y)].f = 0xff;
		m_image[loc(x,y)].b = 0xff;
		m_image[loc(x,y)].r = 0xff;
	}
}

/* --------------------------------- selection ------------------------------ */

void WidgetLayer::pasteClipboard()
{
	insertSelection();
}


void WidgetLayer::insertSelection()
{
	QString text = QApplication::clipboard()->text();
	if ( ! text.isNull() )
	{
		text.replace( QRegExp( "\n" ), "\r" );
		insertText( text );
		// selection should be unselected
		emit selectionCleared();
	}
}

void WidgetLayer::insertText( QString text )
{
	// text is inserted as key event
	QKeyEvent e( QEvent::KeyPress, 0, -1, 0, text);
	emit keyPressed( &e );
}

void WidgetLayer::onClearSelection()
{
	emit selectionCleared();
}

void WidgetLayer::setSelection( const QString& text )
{
	// why get the clipboard, we have it as instance var...
	QObject *m_clipboard = QApplication::clipboard();

	// we know, that cliboard changes, when we change clipboard
	QObject::disconnect( (QObject*) m_clipboard, SIGNAL( dataChanged() ),
			(QObject*) this, SLOT( selectionCleared() ) );
	
	QApplication::clipboard()->setText( text );

	QObject::connect( (QObject*) m_clipboard, SIGNAL( dataChanged() ),
			(QObject*) this, SLOT( selectionCleared() ) );
}
