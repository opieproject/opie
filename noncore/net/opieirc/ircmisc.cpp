#include <opie/colordialog.h>
#include <qlayout.h>
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

