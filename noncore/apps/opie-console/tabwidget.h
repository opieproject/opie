#ifndef OPIE_TAB_WIDGET_H
#define OPIE_TAB_WIDGET_H

#include <qmap.h>
#include <qtabwidget.h>

#include "session.h"
/**
 * This is our central tab widget
 * we can add sessions here
 */
class TabWidget : QTabWidget{
    Q_OBJECT
public:
    TabWidget(QWidget *parent, const char* name );
    ~TabWidget();
    void add( Session* );
    void remove( Session* );

signals:
    void activated(Session* ses );
private slots:
    void slotCurChanged( QWidget* wid );
private:
    QMap<QWidget*, Session*> m_map;
};

#endif
