
#include "tabwidget.h"

TabWidget::TabWidget( QWidget* parent, const char* name )
    : OTabWidget( parent, name ) {
    connect(this, SIGNAL( currentChanged(QWidget*) ),
            this, SLOT( slotCurChanged(QWidget*) ) );
}

TabWidget::~TabWidget() {
}

void TabWidget::add( Session* ses ) {
    qWarning("session ses " + ses->name() );
    if ( !ses->widgetStack() ) return;
    //reparent( ses->widgetStack(), QPoint() );
    addTab( ses->widgetStack(), "console/konsole", ses->name() );
    //addTab( ses->widgetStack(), ses->name() );
    m_map.insert( ses->widgetStack(), ses );
}

void TabWidget::remove( Session* ses ) {
    m_map.remove( ses->widgetStack() );
    removePage( ses->widgetStack() );
}

void TabWidget::slotCurChanged( QWidget* wid ) {
    QMap<QWidget*, Session*>::Iterator it;
    it = m_map.find( wid );
    if ( it == m_map.end() ) {
        return;
    }

    emit activated( it.data() );
}
void TabWidget::setCurrent( Session* ses ) {
    if (!ses )
        return;

    //showPage( ses->widgetStack() );
    setCurrentTab( ses->widgetStack() );
}


