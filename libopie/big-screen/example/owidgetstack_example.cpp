/*
 * You may use, modify and distribute this example without any limitation
 */

#include <qaction.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qsignalmapper.h>

#include <qpe/resource.h>

#include "../owidgetstack.h"

#include "owidgetstack_example.h"

#include <qpe/qpeapplication.h>
#include <opie/oapplicationfactory.h>

OPIE_EXPORT_APP( OApplicationFactory<StackExample> )

StackExample::StackExample( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    m_stack = new OWidgetStack( this );
    setCentralWidget( m_stack );

    /* nice Signal Mapper ;) */
    QSignalMapper *sm = new QSignalMapper(this);
    connect(sm, SIGNAL(mapped(int) ), m_stack, SLOT(raiseWidget(int)) );

    /* toolbar first but this should be known from the other examples */
    setToolBarsMovable( false );

    /* only a menubar here */
    QToolBar* holder = new QToolBar( this );
    holder->setHorizontalStretchable( true );

    QMenuBar *bar = new QMenuBar( holder );
    QPopupMenu *menu = new QPopupMenu( this );

    QAction* a = new QAction( tr("Show MainWidget"), Resource::loadPixmap("zoom"),
                              QString::null, 0, this, 0 );
    sm->setMapping(a, 1 );
    connect(a, SIGNAL(activated() ),
            sm, SLOT(map() ) );
    a->addTo( menu );

    a = new QAction( tr("Show Details Small"), Resource::loadPixmap("zoom"),
                     QString::null, 0, this, 0 );
    sm->setMapping(a, 2 );
    connect(a, SIGNAL(activated() ),
            sm, SLOT(map() ) );
    a->addTo( menu );

    a = new QAction( tr("Show Details More"), Resource::loadPixmap("zoom"),
                     QString::null, 0, this, 0 );
    sm->setMapping(a, 3 );
    connect(a, SIGNAL(activated() ),
            sm, SLOT(map() ) );
    a->addTo( menu );

    a =  new QAction( tr("Show Details All"), Resource::loadPixmap("zoom"),
                      QString::null, 0, this, 0 );
    sm->setMapping(a, 4 );
    connect(a, SIGNAL(activated() ),
            sm, SLOT(map() ) );

    bar->insertItem( tr("Actions"), menu );

    /* now the gui */

    /* first widget, main widget */
    QWidget * wid = new QWidget( m_stack );
    QGridLayout *grid = new QGridLayout(wid, 2, 2 );

    QPushButton *btn = new QPushButton( tr("Show Details Small"), wid, "details1" );
    sm->setMapping(btn, 2 );
    connect(btn, SIGNAL(clicked()), sm, SLOT(map() ) );
    grid->addWidget( btn, 0, 0 );

    btn = new QPushButton( tr("Show Details Medium"), wid, "details2");
    sm->setMapping(btn, 3 );
    connect(btn, SIGNAL(clicked()), sm, SLOT(map() ) );
    grid->addWidget( btn, 0, 1 );

    btn = new QPushButton( tr("Show Details All"), wid, "details3");
    sm->setMapping(btn, 4 );
    connect(btn, SIGNAL(clicked()), sm, SLOT(map() ) );
    grid->addWidget( btn, 1, 1 );

    m_stack->addWidget( wid, 1 );
    m_main = wid;

    QLabel *lbl = new QLabel(m_stack );
    lbl->setText(tr("Only small Details are shown here. Määh") );
    m_stack->addWidget( lbl, 2 );

    lbl = new QLabel( m_stack );
    lbl->setText( tr("Some more details....Wo ist das Schaf?") );
    m_stack->addWidget( lbl, 3 );

    lbl = new QLabel( m_stack );
    lbl->setText( tr("<qt>Ne nicht in Bayerisch Gmain sondern in Berlin<br>Vermiss und meine Augen werden nicht eckig, da mein Bildschirm abgerundet ist<br>Es lebe Hamburg Süd,weiss du, verstehst du? ;)<br>Susi ist dOOf, es lebe die Ofenecke...", "hard to translate that") );
    m_stack->addWidget( lbl, 4 );


    /* THE signal mapper does all the magic */
    m_stack->raiseWidget( m_main );
}


StackExample::~StackExample() {

}



void StackExample::closeEvent( QCloseEvent* ev) {
    /* if the close even came when we displayed a details */
    if (m_stack->visibleWidget() != m_main ) {
        m_stack->raiseWidget( m_main );
        ev->ignore();
        return;
    }

    ev->accept();
}
