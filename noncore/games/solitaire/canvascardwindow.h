/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef CANVAS_CARD_WINDOW_H 
#define CANVAS_CARD_WINDOW_H


#include <qmainwindow.h>
#include <qcanvas.h>


// Determines if we resize to mega small on really low res screens
extern bool smallFlag;


class CanvasCardGame;
class QPopupMenu;


class CanvasCardWindow : public QMainWindow {
    Q_OBJECT

public:
    CanvasCardWindow(QWidget* parent=0, const char* name=0, WFlags f=0);
    virtual ~CanvasCardWindow();

public slots:
    void setCardBacks();
    void changeCardBacks();
    void snapToggle();
    void drawnToggle();

private slots:
    void initFreecell();
    void initPatience();

protected:
    virtual void resizeEvent(QResizeEvent *e);

    void updateDraw();
private:
    QCanvas canvas;
    bool snapOn;
    bool drawThree;
    int drawId;
    int cardBack;
    int gameType;
    CanvasCardGame *cardGame;

    QPopupMenu* options;
    QPopupMenu* settings;
    int dbf_id;
    int snap_id;
};


#endif

