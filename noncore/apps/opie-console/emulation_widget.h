// opie console includes
#include "widget_layer.h"
#include "profile.h"
#include "common.h"

// qt includes
#include <qwidget.h>
#include <qpainter.h>
#include <qstring.h>
#include <qarray.h>
#include <qrect.h>



/**
 * EmulationWidget
 * simple implementation of EmulationLayer
 * (doesn't support scrollbar, history, etc, yet)
 */
class EmulationWidget : public WidgetLayer
{ Q_OBJECT

public:

    /**
     * constructor
     * @param const Profile& config, the configuration
     * @param QWidget* parent, parent widget
     * @param const char* name, the name of the widget
     */
    EmulationWidget( const Profile& config, QWidget *parent=0, const char *name =0 );

    /**
     * destructor
     */
    ~EmulationWidget();

    /**
     * sets the image
     * @param QArray<Character> const newimg, the new image
     * @param int lines, lines of the new image
     * @param int columns, columns of the new image
     */
    virtual void setImage( QArray<Character> const newimg, int columns, int lines );

    /**
     * reloads configuration
     * @param const Profile& config, configuration
     */
    virtual void reloadConfig( const Profile& config );

    /**
     * sets the scrollbar (not yet implemented)
     */
    virtual void setScroll( int cursor, int slines );

    /**
     * scrolls (not yet implemented)
     * @param int value, scroll by this value
     */
    virtual void scroll( int value );

    virtual QSize calcSize( int cols, int lins ) const;

protected:

    /**
     * calculates current image bounds
     */
    virtual void calcGeometry();


    void paintEvent( QPaintEvent* event );

    /**
     * @param const ColorEntry* table, the new color table
     */
    void setColorTable( const ColorEntry table[] );

    /**
     * draws a String
     * @param QString& string, string to be drawn
     * @param QPainter& painter, painter, that should draw
     * @param QRect rect, rect to be drawn into
     * @param Character attr, attributes of Characters
     * @param bool usePixmap, if to use the background pixmap (currently not supported)
     * @param bool clear, if rect should be cleared
     */
    void drawAttrString( QString& string, QPainter& painter, QRect rect, Character attr, bool pm, bool clear );

protected:

    enum ScrollLocation
      {
	SCRNONE,
	SCRLEFT,
	SCRIGHT
      };

	int f_height;
	int f_width;
	int f_ascent;
	int m_blX;
	int m_blY;
	int m_brX;

	int m_bY;
	int m_bX;
	QScrollBar* m_scrollbar;

	ScrollLocation scrollLoc;

//	ColorEntry* color_table;

	bool blinking;
};
