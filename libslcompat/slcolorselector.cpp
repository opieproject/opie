#include "slcolorselector.h"

SlColorTable::SlColorTable( QWidget *parent, const char *name,WFlags f,
                            int r, int c, const QRgb *a )
             :QTableView( parent, name, f )
{
}

SlColorTable::~SlColorTable()
{
}

QSize SlColorTable::sizeHint() const
{
}

void SlColorTable::paintCell(QPainter*,int,int)
{
}

void SlColorTable::mousePressEvent(QMouseEvent*)
{
}

void SlColorTable::mouseMoveEvent(QMouseEvent*)
{
}

void SlColorTable::mouseReleaseEvent(QMouseEvent*)
{
}

void SlColorTable::keyPressEvent(QKeyEvent*)
{
}


/*================================================================*/

SlColorSelector::SlColorSelector(bool isTransparentAccept, QWidget *parent, const char *name, WFlags f)
                :QVBox( parent, name, f )
{
}

SlColorSelector::~SlColorSelector()
{
}

/*================================================================*/

SlColorSelectorPopup::SlColorSelectorPopup(bool isTransparentAccept, QWidget *parent, const char *name, WFlags f)
                     :OColorPopupMenu( QColor(), parent, name )
{
}


SlColorSelectorPopup::~SlColorSelectorPopup()
{
}

void SlColorSelectorPopup::setCurrentColor(QRgb)
{
}

