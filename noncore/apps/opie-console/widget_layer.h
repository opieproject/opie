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



class WidgetLayer : public QObject
{ QObject

public:
	/**
	 * constructor
	 */
	WidgetLayer();

	/**
	 * destructor
	 */
	virtual ~WidgetLayer();

	/**
	 * sets the image
	 */
	void setImage( const Character* const newimg, int lines, int colums );

	/**
	 * annoy the user
	 */
	void bell();

	/**
	 * return the lines count
	 */
	int lines()	{ return _lines; }
	
	/**
	 * return the columns count
	 */
	int columns()	{ return _columns }

	/**
	 * copy selection into clipboard, etc
	 */
	void emitSelection();

	/**
	 * set selection (clipboard) to text
	 */
	void setSelection( QString &text )

	/**
	 * paste content of clipboard
	 */
	void pasteClipboard();


signals:

	/**
	 * key was pressed
	 */
	keyPressed( QKeyEvent *e );

	/**
	 * whenever Mouse selects something
	 * 0	left Button
	 * 3	Button released
	 * // numbering due to layout in old TEWidget
	 */
	mousePressed( int button, int x, int y );

	/**
	 * size of image changed
	 */
	imageSizeChanged( int lines, int columns );

	/**
	 * cursor in history changed
	 */
	historyCursorChanged( int value );

	/**
	 * selection should be cleared
	 */
	void selectionCleared();
	
	/**
	 * selection begin
	 */
	void selectionBegin( const int x, const int y )

	/**
	 * selection extended
	 *  (from begin s.a. to x, y)
	 */
	void selectionExtended( const int x, const int y );

	/**
	 * selection end
	 *  bool: preserve line breaks in selection
	 */
	void selectionEnd( const bool lineBreakPreserve );

};


