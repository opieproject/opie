#ifndef OPIE_FILESELECTOR_MAIN_H
#define OPIE_FILESELECTOR_MAIN_H

#include <qwidget.h>

class QGridLayout;
class QWidgetStack;
class OFileSelectorMain : public QWidget {
    Q_OBJECT
public:
    OFileSelectorMain( QWidget* parent );
    ~OFileSelectorMain();
    void setToolbar( QWidget* tool );
    void setWidget( QWidget* view );
private:
    void init();
    void add();
    QGridLayout *m_lay;
    QWidget* m_tool;
    QWidgetStack *m_stack;
};

#endif
