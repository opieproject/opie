
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

void TodoShow::showNext() {
    if (m_win)
        m_win->slotShowNext();
}
void TodoShow::showPrev() {
    if (m_win)
        m_win->slotShowPrev();
}
