#include <qevent.h>
#include <qapplication.h>
#include <qaction.h>

#include <qpe/resource.h>
#include "quick_button.h"

QuickButton::QuickButton(QWidget *parent) : QFrame(parent) {

    QAction *a;

    // Button Commands
    a = new QAction( tr("Enter"), Resource::loadPixmap( "console/enter" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitEnter() ) );
    a->addTo( parent );
    a = new QAction( tr("Space"), Resource::loadPixmap( "console/space" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitSpace() ) );
    a->addTo( parent );
    a = new QAction( tr("Tab"), Resource::loadPixmap( "console/tab" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitTab() ) );
    a->addTo( parent );
    a = new QAction( tr("Up"), Resource::loadPixmap( "console/up" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitUp() ) );
    a->addTo( parent );
    a = new QAction( tr("Down"), Resource::loadPixmap( "console/down" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitDown() ) );
    a->addTo( parent );
    a = new QAction( tr("Paste"), Resource::loadPixmap( "paste" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitPaste() ) );
    a->addTo( parent );


}

QuickButton::~QuickButton() {
}


void QuickButton::hitEnter() {
    emit keyPressed( QKeyEvent::KeyPress, Qt::Key_Enter, 0, 1, 0);
}

void QuickButton::hitSpace() {
    emit keyPressed( QKeyEvent::KeyPress, Qt::Key_Space, 0, 1, 0);
}

void QuickButton::hitTab() {
    emit keyPressed( QKeyEvent::KeyPress, Qt::Key_Tab, 0, 1, 0);
}

void QuickButton::hitPaste() {
    /*
    TEWidget* te = getTe();
    if (te != 0) {
    te->pasteClipboard();
    }
    */
}

void QuickButton::hitUp() {
    emit keyPressed( QKeyEvent::KeyPress, Qt::Key_Up, 0, 1, 0);
}

void QuickButton::hitDown() {
    emit keyPressed( QKeyEvent::KeyPress, Qt::Key_Down, 0, 1, 0);
}

