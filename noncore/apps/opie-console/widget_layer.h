/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [widget_layer.h]         Widget Layer                                      */
/*                                                                            */
/* -------------------------------------------------------------------------- */

// proposal of a widget Layer in opie-console
//
// fellow devels:
//   just mail me (ibotty@web.de), what you additionally need from the main widget 
//   (or say in chat)

#ifndef WIDGET_LAYER_H
#define WIDGET_LAYER_H

// qt includes
#include <qapplication.h>
#include <qtimer.h>
#include <qkeycode.h>
#include <qclipboard.h>


// opie-console includes
#include "session.h"
#include "common.h"

class WidgetLayer : public QObject
{ Q_OBJECT

public:

	/**
	 * constructor
	 */
	WidgetLayer( QObject *parent=0, const char *name=0 );

	/**
	 * destructor
	 */
	virtual ~WidgetLayer();

public:
	/**
	 * sets the image
	 */
	virtual void setImage( const Character* const newimg, int lines, int colums );

	/**
	 * annoy the user
	 */
	void bell();

	/**
	 * return the lines count
	 */
	int lines()	{ return m_lines; }
	
	/**
	 * return the columns count
	 */
	int columns()	{ return m_columns; }

	/**
	 * insert current selection (currently this is only the clipboard)
	 */
	void insertSelection();

	/**
	 * insert text
	 */
	void insertText( QString text );
	/**
	 * set selection (clipboard) to text
	 */
	void setSelection( const QString &text );

	/**
	 * paste content of clipboard
	 */
	void pasteClipboard();


signals:

	/**
	 * key was pressed
	 */
	void keyPressed( QKeyEvent *e );

	/**
	 * whenever Mouse selects something
	 * 0	left Button
	 * 3	Button released
	 * // numbering due to layout in old TEWidget
	 */
	void mousePressed( int button, int x, int y );

	/**
	 * size of image changed
	 */
	void imageSizeChanged( int lines, int columns );

	/**
	 * cursor in history changed
	 */
	void historyCursorChanged( int value );

	/**
	 * selection should be cleared
	 */
	void selectionCleared();
	
	/**
	 * selection begin
	 */
	void selectionBegin( const int x, const int y );

	/**
	 * selection extended
	 *  (from begin (s.a.) to x, y)
	 */
	void selectionExtended( const int x, const int y );

	/**
	 * selection end
	 *  bool: preserve line breaks in selection
	 */
	void selectionEnd( const bool lineBreakPreserve );



// protected methods
protected:
	
	// image operations

	/**
	 * changes image, to suit new size
	 * TODO: find meaningful name!
	 */
	void propagateSize();

	/**
	 *
	 */
	virtual void calcGeometry();

	/**
	 * makes an empty image
	 */
	void makeImage();

	/**
	 * clears the image
	 */
	void clearImage();
	
protected slots:
	
	/**
	 * clear selection
	 */
	void onClearSelection();


// protected vars
protected:
	
	/**
	 * current Session
	 */
	Session *m_session;

	/**
	 * current character image
	 */
	QArray<Character> m_image;

	/**
	 * lines count
	 */
	int m_lines;

	/**
	 * columns count
	 */
	int m_columns;

	/**
	 * clipboard
	 */
	QClipboard* m_clipboard;

	/**
	 * whether widget was resized 
	 */
	bool m_resizing;
};

#endif // WIDGET_LAYER_H
