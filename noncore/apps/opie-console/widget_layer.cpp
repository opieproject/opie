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



WidgetLayer::WidgetLayer( const Profile &config, QWidget *parent, const char *name  ) : QFrame( parent, name )
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
	
	// just for demonstrating
	//m_image = QArray<Character>( m_lines * m_columns ); 
	m_image = QArray<Character>( 1 );

}


WidgetLayer::~WidgetLayer()
{
	// clean up
	delete m_image;
}

/* --------------------------------- audio ---------------------------------- */

void WidgetLayer::bell()
{
	QApplication::beep();
}

bool WidgetLayer::eventFilter( QObject *obj, QEvent *e )
{
    if ( (e->type() == QEvent::Accel ||
          e->type() == QEvent::AccelAvailable ) && qApp->focusWidget() == this )  {
        static_cast<QKeyEvent *>( e )->ignore();
        return true;
    }
    if ( obj != this /* when embedded */ && obj != parent() /* when standalone */ )
        return FALSE; // not us
    if ( e->type() == QEvent::Wheel)  {
        QApplication::sendEvent( m_scrollbar, e);
    }

#ifdef FAKE_CTRL_AND_ALT
    static bool control = FALSE;
    static bool alt = FALSE;
//    qDebug(" Has a keyboard with no CTRL and ALT keys, but we fake it:");
    bool dele=FALSE;
    if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease ) {
        QKeyEvent* ke = (QKeyEvent*)e;
        bool keydown = e->type() == QEvent::KeyPress || ke->isAutoRepeat();
        switch (ke->key()) {
          case Key_F9: // let this be "Control"
              control = keydown;
              e = new QKeyEvent(QEvent::KeyPress, Key_Control, 0, ke->state());
              dele=TRUE;
              break;
          case Key_F13: // let this be "Alt"
	      alt = keydown;
              e = new QKeyEvent(QEvent::KeyPress, Key_Alt, 0, ke->state());
              dele=TRUE;
              break;
          default:
              if ( control ) {
                  int a = toupper(ke->ascii())-64;
                  if ( a >= 0 && a < ' ' ) {
                      e = new QKeyEvent(e->type(), ke->key(),
                                        a, ke->state()|ControlButton,
QChar(a,0));
                      dele=TRUE;
                  }
              }
              if ( alt ) {
                  e = new QKeyEvent(e->type(), ke->key(),
                                    ke->ascii(), ke->state()|AltButton, ke->text());
                  dele=TRUE;
              }
        }
    }
#endif

    if ( e->type() == QEvent::KeyPress )  {
        QKeyEvent* ke = (QKeyEvent*)e;
        //actSel=0; // Key stroke implies a screen update, so Widget won't
          // know where the current selection is.

//     qDebug("key pressed is 0x%x",ke->key());

        if( ke->state() == ShiftButton && ke->key() == Key_Tab) { //lets hardcode this sucker
            insertText("\\"); // expose
        } else
            emit keyPressed( ke ); // expose
        ke->accept();
#ifdef FAKE_CTRL_AND_ALT
        if ( dele ) delete e;
#endif
        return true;               // stop the event
    }
    if ( e->type() == QEvent::Enter )  {
        QObject::disconnect( (QObject*)cb, SIGNAL(dataChanged()),
                             this, SLOT(onClearSelection()) );
    }
    if ( e->type() == QEvent::Leave )  {
        QObject::connect( (QObject*)cb, SIGNAL(dataChanged()),
                          this, SLOT(onClearSelection()) );
    }
    return QFrame::eventFilter( obj, e );
}


/* --------------------------------- screen --------------------------------- */


void WidgetLayer::propagateSize()
{
	QArray<Character> oldimage = m_image.copy();
	int oldlines = m_lines;
	int oldcolumns = m_columns;
	
	makeImage();
	
	// copy old image, to reduce flicker
	if ( ! oldimage.isEmpty() )
	{
		int lins = QMIN( oldlines, m_lines );
		int cols = QMIN( oldcolumns, m_columns );
		for ( int lin = 0; lin < lins; ++lin )
		{
			memcpy( (void*) &m_image[m_columns*lin],
				(void*) &oldimage[oldcolumns*lin],
				cols*sizeof( Character ) );
		}
	}
	else
		clearImage();
	
	delete oldimage;

	m_resizing = true;
	emit imageSizeChanged( m_lines, m_columns );
	m_resizing = false;
}

void WidgetLayer::makeImage()
{
	calcGeometry();
	m_image = QArray<Character>( m_columns * m_lines );
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


/////////
// special font characters
/////////
unsigned short vt100_graphics[32] =
{ // 0/8     1/9    2/10    3/11    4/12    5/13    6/14    7/15
  0x0020, 0x25C6, 0x2592, 0x2409, 0x240c, 0x240d, 0x240a, 0x00b0,
  0x00b1, 0x2424, 0x240b, 0x2518, 0x2510, 0x250c, 0x2514, 0x253c,
  0xF800, 0xF801, 0x2500, 0xF803, 0xF804, 0x251c, 0x2524, 0x2534,
  0x252c, 0x2502, 0x2264, 0x2265, 0x03C0, 0x2260, 0x00A3, 0x00b7
};

