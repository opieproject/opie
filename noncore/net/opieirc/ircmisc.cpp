#include <qlayout.h>
#include <stdio.h>
#include "ircmisc.h"

IRCTabBar::IRCTabBar(QWidget *parent, const char *name) : QTabBar(parent, name) {
}

int IRCTabBar::insertTab(QTab *tab, int index) {
    /* FIXME: find some nicer way to do this */
    QExtTab *ext = new QExtTab();
    ext->color = black;
    ext->label = tab->text();
    ext->r = tab->rect();
    ext->enabled = tab->isEnabled();
    ext->iconset = tab->iconSet();
    delete tab;
    return QTabBar::insertTab(ext, index);
}

void IRCTabBar::setTabColor(int index, QColor color) {
    ((QExtTab *)tab(index))->color = color;
    update();
}

void IRCTabBar::paintLabel(QPainter* p, const QRect& br, QTab* t, bool focus) const {
    QRect r = br;
    QTabBar::paintLabel(p, br, t, focus);
    if (t->id == currentTab())
        r.setBottom(r.bottom() - style().defaultFrameWidth());
    p->setPen(((QExtTab *)t)->color);
    p->drawText(r, AlignCenter | ShowPrefix, t->label);
}

IRCTabWidget::IRCTabWidget(QWidget *parent, const char *name) : QTabWidget(parent, name) {
    setTabBar(new IRCTabBar(this, "tab control"));
}

void IRCTabWidget::setTabColor(int index, QColor color) {
    ((IRCTabBar *)tabBar())->setTabColor(index, color);
}


IRCHistoryLineEdit::IRCHistoryLineEdit(QWidget *parent, const char *name) : QLineEdit(parent, name) {
    m_index = -1;
    installEventFilter(this);
}

void IRCHistoryLineEdit::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (key == Key_Up) {
        if (m_history.count() > 0 && m_index < (signed int)m_history.count()-1) {
            m_index++;
            setText(m_history[m_index]);
        }
    } else if (key == Key_Down) {
        if (m_history.count() > 0 && m_index > 0) {
            m_index--;
            setText(m_history[m_index]);
        }
        if (m_index == 0) {
            m_index = -1;
            setText("");
        }
    } else if (key == Key_Return) {
        m_history.prepend(text());
        m_index = -1;
    } else if (key == Key_Tab) {
        printf("got tab\n");
        return;
    }
    QLineEdit::keyPressEvent(event);
}

bool IRCHistoryLineEdit::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *k = (QKeyEvent *) event;
        /* Catch tab characters */
        if (k->key() == Key_Tab) {
            qDebug("tab!");
            return TRUE;
        }
    }
    return QLineEdit::eventFilter(object, event);
}
