#include "viewmodebutton.h"

#include <qtoolbar.h>
#include <qpopupmenu.h>

ViewModeButton::ViewModeButton( QToolBar* bar )
    : QToolButton( bar )
{
    setIconSet( Resource::loadIconSet( "toys" ) );
    QPopupMenu *pop= new QPopupMenu( this );
    pop->setCheckable( true );
    pop->insertItem( tr("Thumbnail and Imageinfo"), 0 );
    pop->insertItem( tr("Thumbnail and Name" ), 1 );
    pop->insertItem( tr("Name Only" ), 2 );
    connect(pop, SIGNAL(activated(int)),
            this, SIGNAL(changeMode(int)) );


    setPopup( pop );
}

ViewModeButton::~ViewModeButton() {

}
