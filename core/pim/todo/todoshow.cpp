
#include "mainwindow.h"
#include "todoshow.h"

using namespace Todo;

TodoShow::TodoShow(MainWindow* win) {
    m_win = win;
}
TodoShow::~TodoShow() {
}
void TodoShow::escapeView() {
    if (m_win )
        m_win->slotReturnFromView();
}

