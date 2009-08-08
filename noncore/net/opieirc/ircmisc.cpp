#include "ircmisc.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oconfig.h>
#include <opie2/okeyconfigmanager.h>
using namespace Opie::Core;


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



static OKeyConfigManager* s_manager = 0;
OKeyConfigManager* IRCHistoryLineEdit::keyConfigInstance() {
    if ( !s_manager ) {
        /*
         * black list with the DeviceButtons as default
         * because we do not grab the keyboard and they
         * wouldn't work
         */
        OKeyPair::List blackList = OKeyPair::hardwareKeys();
        blackList.append( OKeyPair::returnKey() );
        blackList.append( OKeyPair::leftArrowKey() );
        blackList.append( OKeyPair::upArrowKey() );
        blackList.append( OKeyPair::downArrowKey() );

        s_manager = new OKeyConfigManager(new OConfig("opieirc-keys"),
                                          "keys", blackList,
                                          false, 0, "irc_history_line_keyconfigm" );
        s_manager->addKeyConfig( OKeyConfigItem( tr("Next Tab"), "next_tab", QPixmap(),
                                                 KeyNextTab, OKeyPair(Qt::Key_N, Qt::ControlButton) ));
        s_manager->addKeyConfig( OKeyConfigItem( tr("Previous Tab"), "prev_tab", QPixmap(),
                                                 KeyPrevTab, OKeyPair(Qt::Key_P, Qt::ControlButton) ));
        s_manager->addKeyConfig( OKeyConfigItem( tr("Close Tab"), "close_tab",
                                                 QPixmap(), KeyCloseTab,
                                                 OKeyPair(Qt::Key_C, Qt::ControlButton) ));
        s_manager->load();
    }

    return s_manager;
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
    } 
    else if (key == Key_Down) {
        if (m_history.count() > 0 && m_index > 0) {
            m_index--;
            setText(m_history[m_index]);
        }
        if (m_index == 0) {
            m_index = -1;
            setText("");
        }
    } 
    else if (key == Key_Return) {
        m_history.prepend(text());
        m_index = -1;
    }

    switch( keyConfigInstance()->handleKeyEventId( event ) ) {
        case KeyNextTab:
            emit nextTab();
            return;
        case KeyPrevTab:
            emit prevTab();
            return;
        case KeyCloseTab:
            emit closeTab();
        default:
            break;
    }

    QLineEdit::keyPressEvent(event);
}

bool IRCHistoryLineEdit::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *k = (QKeyEvent *) event;
        /* Catch tab characters */
        if (k->key() == Key_Tab) {
            emit nextTab();
            return TRUE;
        }
    }
    return QLineEdit::eventFilter(object, event);
}


void IRCHistoryLineEdit::setEditFocus() {
    setActiveWindow();
    setFocus();
}
