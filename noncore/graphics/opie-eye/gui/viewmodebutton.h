/*
 * GPLv2 only
 * zecke@handhelds.org
 */

#ifndef PHUNK_VIEW_MODE_BUTTON_H
#define PHUNK_VIEW_MODE_BUTTON_H

#include <qpe/resource.h>

#include <qtoolbutton.h>

class ViewModeButton : public QToolButton {
    Q_OBJECT
public:
    ViewModeButton( QToolBar* );
    ~ViewModeButton();

signals:
    void changeMode( int );
};

#endif
