#include <opie/colordialog.h>
#include <qlayout.h>
#include <stdio.h>
#include "ircmisc.h"

IRCColorLabel::IRCColorLabel(QColor color, QWidget *parent, const char *name, WFlags f) : QLabel(parent, name, f) {
    m_color = color;
    setAlignment(AlignVCenter | AlignCenter);
    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setBackgroundColor(m_color);
}

void IRCColorLabel::mousePressEvent(QMouseEvent *) {
    m_color = OColorDialog::getColor(m_color);
    setBackgroundColor(m_color);
}

QColor IRCColorLabel::color() {
    return m_color;
}

IRCFramedColorLabel::IRCFramedColorLabel(QColor color, QWidget *parent, const char *name, WFlags f) : QWidget(parent, name, f) {
    QVBoxLayout *layout = new QVBoxLayout(this, 10, 0);
    m_label = new IRCColorLabel(color, this);
    layout->addWidget(m_label);
}

QColor IRCFramedColorLabel::color() {
    return m_label->color();
}


IRCTabBar::IRCTabBar(QWidget *parent, const char *name) : QTabBar(parent, name) {
}

int IRCTabBar::insertTab(QTab *tab, int index = -1) {
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
    }
    QLineEdit::keyPressEvent(event);
}
