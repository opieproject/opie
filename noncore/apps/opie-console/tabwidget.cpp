
#include "tabwidget.h"

TabWidget::TabWidget( QWidget* parent, const char* name )
    : OTabWidget( parent, name ) {
    connect(this, SIGNAL( currentChanged(QWidget*) ),
            this, SLOT( slotCurChanged(QWidget*) ) );
}

TabWidget::~TabWidget() {
}

void TabWidget::add( Session* ses ) {
    if ( !ses->widget() ) return;
    addTab( ses->widget(), "console/konsole", ses->name() );
    m_map.insert( ses->widget(), ses );
}

void TabWidget::remove( Session* ses ) {
    m_map.remove( ses->widget() );
    removePage( ses->widget() );
}

void TabWidget::slotCurChanged( QWidget* wid ) {
    QMap<QWidget*, Session*>::Iterator it;
    it = m_map.find( wid );
    if ( it == m_map.end() ) {
        return;
    }

    emit activated( it.data() );
}


