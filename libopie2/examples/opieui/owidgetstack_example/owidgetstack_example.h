/*
 * You may use, modify and distribute this example without any limitation
 */

#ifndef O_STACK_EXAMPLE_SIMPLE_H
#define O_STACK_EXAMPLE_SIMPLE_H

#include <qmainwindow.h>
#include <opie2/owidgetstack.h>

using namespace Opie;

class StackExample : public QMainWindow {
    Q_OBJECT
public:
    StackExample( QWidget* paren, const char* name, WFlags fl );
    ~StackExample();
    static QString appName() { return QString::fromLatin1("owidgetstack-example"); }

protected:
    void closeEvent( QCloseEvent* e );
private:
    OWidgetStack* m_stack;
    QWidget* m_main;

};

#endif
