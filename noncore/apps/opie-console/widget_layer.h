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
#include <qframe.h>
#include <qarray.h>
#include <qtimer.h>
#include <qkeycode.h>
#include <qclipboard.h>


// opie-console includes
#include "session.h"
#include "common.h"
#include "profile.h"
/*
 * given a pseudo location ( column, line ),
 * returns the actual index, in the QArray<Character>
 */
#define loc(X,Y) ((Y)*m_columns+(X))




class WidgetLayer : public QFrame
{ Q_OBJECT

public:

	/**
	 * constructor
	 * @param const Profile &config, the configuration for this widget
	 * @param QWidget *parent, the parent widget
	 * @param const char *name, the name of the widget, defaults to ""
	 */
	WidgetLayer( const Profile& config, QWidget *parent=0, const char *name=0 );

	/**
	 * destructor
	 */
	virtual ~WidgetLayer();

public:
	/**
	 * sets the image
	 * @param QArray<Character> const newimg, the new image
	 * @param int lines, lines count of newimg
	 * @param int columns, columns count of newimg
	 */
	virtual void setImage( QArray<Character> const newimg, int lines, int colums ) = 0;

	/**
	 * annoy the user
	 */
	void bell();

	/**
	 * @return int m_lines, the lines count
	 */
	int lines()	{ return m_lines; }

	/**
	 * @return int m_columns, the columns count
	 */
	int columns()	{ return m_columns; }

	/**
	 * insert current selection (currently this is only the clipboard)
	 */
	void insertSelection();

	/**
	 * insert text
	 * @param QString text, the text to be inserted
	 */
	void insertText( QString text );

	/**
	 * set selection (clipboard) to text
	 * @param const QString &text, the text to be selected
	 */
	void setSelection( const QString &text );

	/**
	 * paste content of clipboard
	 */
	void pasteClipboard();


	/**
	 * reload configuration
	 */
	virtual void reloadConfig() = 0;


signals:

	/**
	 * key was pressed
	 */
	void keyPressed( QKeyEvent *e );

	/**
	 * whenever Mouse selects something
	 * @param int button, the button that us pressed :
	 * 		0	left Button
	 * 		3	Button released
	 * @param int x, x position
	 * @param int y, y position
	 *
	 * // numbering due to layout in old TEWidget
	 */
	void mousePressed( int button, int x, int y );

	/**
	 * size of image changed
	 * @param int lines, line count of new size
	 * @param int columns, column count of new size
	 */
	void imageSizeChanged( int lines, int columns );

	/**
	 * cursor in history changed
	 * @param int value, value of history cursor
	 */
	void historyCursorChanged( int value );

	/**
	 * selection should be cleared
	 */
	void selectionCleared();

	/**
	 * selection begin
	 * @param const int x, x position
	 * @param const int y, y position
	 */
	void selectionBegin( const int x, const int y );

	/**
	 * selection extended
	 *  (from begin (s.a.) to x, y)
	 *  @param const int x, x position
	 *  @param const int y, y position
	 */
	void selectionExtended( const int x, const int y );

	/**
	 * selection end
	 * @param const bool lineBreakPreserve, preserve line breaks in selection
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
	 *determines count of lines and columns
	 */
	virtual void calcGeometry() = 0;

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
	 *
	 * a Character at loc( column, line )
	 * has the actual index:
	 *  ix = line * m_columns + column;
	 *
	 * use loc( x, y ) macro to access.
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
	 * whether widget is resizing
	 */
	bool m_resizing;
};

#endif // WIDGET_LAYER_H
