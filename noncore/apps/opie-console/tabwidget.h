#ifndef OPIE_TAB_WIDGET_H
#define OPIE_TAB_WIDGET_H

#include <qmap.h>
#include <opie2/otabwidget.h>

#include "session.h"
/**
 * This is our central tab widget
 * we can add sessions here
 */
class TabWidget : public Opie::Ui::OTabWidget{
    Q_OBJECT
public:
    TabWidget(QWidget *parent, const char* name );
    ~TabWidget();
    void add( Session* );
    void remove( Session* );
    void setCurrent( Session* );

signals:
    void activated(Session* ses );
private slots:
    void slotCurChanged( QWidget* wid );
private:
    QMap<QWidget*, Session*> m_map;
};

#endif
