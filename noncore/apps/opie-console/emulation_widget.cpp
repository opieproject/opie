// opie-console includes
#include "emulation_widget.h"
#include "common.h"
#include "widget_layer.h"
#include "profile.h"

// qt includes
#include <qwidget.h>
#include <qarray.h>
#include <qstring.h>
#include <qpainter.h>
#include <qrect.h>


EmulationWidget::EmulationWidget( const Profile& config, QWidget *parent, const char* name ) : WidgetLayer( config, parent, name )
{

}

EmulationWidget::~EmulationWidget()
{
}


void setImage( QArray<Character> const newimg, int lines, int columns )
{
} 

void drawAttrString( QString& string, QPainter &paint, QRect rect, Character attr, bool usePixmap, bool clear )
{


}
    
///////////////////////
// scrollbar
// ////////////////////

void EmulationWidget::scroll( int value )
{
}

void EmulationWidget::setScroll( int cursor, int slines )
{
}
