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
    if (m_colors.size() == (unsigned int)count())
        m_colors.resize(m_colors.size() * 2 + 1);
    if (index == -1)
        m_colors.insert(count(), &black);
    else
        m_colors.insert(index, &black);
    return QTabBar::insertTab(tab, index);
}

void IRCTabBar::setTabColor(int index, const QColor *color) {
    m_colors.insert(index, color);
    update();
}

void IRCTabBar::paintLabel(QPainter* p, const QRect& br, QTab* t, bool focus) const {
    QRect r = br;
    QTabBar::paintLabel(p, br, t, focus);
    if (t->id == currentTab())
        r.setBottom(r.bottom() - style().defaultFrameWidth());
    p->setPen(*m_colors.at(t->id));
    p->drawText(r, AlignCenter | ShowPrefix, t->label);
}

IRCTabWidget::IRCTabWidget(QWidget *parent, const char *name) : QTabWidget(parent, name) {
    setTabBar(new IRCTabBar(this, "tab control"));
}

void IRCTabWidget::setTabColor(int index, const QColor *color) {
    ((IRCTabBar *)tabBar())->setTabColor(index, color);
}
