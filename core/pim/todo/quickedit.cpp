#include "mainwindow.h"
#include "quickedit.h"

using namespace Todo;

// not so interesting part base Implementation
QuickEdit::QuickEdit(MainWindow* main )
    : m_main( main )  {
    m_sig = new QSignal();
}
QuickEdit::~QuickEdit() {
    delete m_sig;
}
QSignal* QuickEdit::signal() {
    return m_sig;
}
MainWindow* QuickEdit::mainWindow() {
    return m_main;
}
void QuickEdit::commit() {
    m_sig->activate();
}
