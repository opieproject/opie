#include "viewmodebutton.h"

#include <opie2/odebug.h>

#include <qtoolbar.h>
#include <qpopupmenu.h>

ViewModeButton::ViewModeButton( QToolBar* bar,int def )
    : QToolButton( bar )
{
    slotChange( def );
    QPopupMenu *pop= new QPopupMenu( this );
    pop->setCheckable( true );
    pop->insertItem( tr("Thumbnail and Imageinfo"), 1 );
    pop->insertItem( tr("Thumbnail and Name" ), 2 );
    pop->insertItem( tr("Name Only" ), 3 );
    connect(pop, SIGNAL(activated(int)),
            this, SIGNAL(changeMode(int)) );
    connect(pop, SIGNAL(activated(int)),
            this, SLOT(slotChange(int)) );


    setPopup( pop );
}

ViewModeButton::~ViewModeButton() {
}

void ViewModeButton::slotChange( int i ) {
    QString name;
    switch( i ) {
    case 1:
        name = "opie-eye/opie-eye-thumb";
        break;
    case 2:
        name = "opie-eye/opie-eye-thumbonly";
        break;
    case 3:
        name = "opie-eye/opie-eye-textview";
        break;
    }

    qWarning("foo %d" +name, i );
    setIconSet( Resource::loadIconSet( name ) );
}
