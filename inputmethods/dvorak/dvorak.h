/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include <qframe.h>
#include "../pickboard/pickboardcfg.h"
#include "../pickboard/pickboardpicks.h"

class QTimer;

class KeyboardConfig : public DictFilterConfig
{
public:
    KeyboardConfig(PickboardPicks* p) : DictFilterConfig(p), backspaces(0) { nrows = 1; }
    virtual void generateText(const QString &s);
    void decBackspaces() { if (backspaces) backspaces--; }
    void incBackspaces() { backspaces++; }
    void resetBackspaces() { backspaces = 0; }
private:
    int backspaces;
};


class KeyboardPicks : public PickboardPicks
{
    Q_OBJECT
public:
    KeyboardPicks(QWidget* parent=0, const char* name=0, WFlags f=0)
	: PickboardPicks(parent, name, f) { }
    void initialise();
    virtual QSize sizeHint() const;
    KeyboardConfig *dc;
};

class Keyboard : public QFrame
{
    Q_OBJECT
public:
    Keyboard( QWidget* parent=0, const char* name=0, WFlags f=0 );

    void resetState();

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent* e);
    void timerEvent(QTimerEvent* e);
    void drawKeyboard( QPainter &p, int key = -1 );

    void setMode(int mode) { useOptiKeys = mode; }

    QSize sizeHint() const;

signals:
    void key( ushort scancode, ushort unicode, ushort modifiers, bool, bool );

private slots:
    void repeat();
    
private:
    int getKey( int &w, int j = -1 );
    void clearHighlight();

    uint shift:1;
    uint lock:1;
    uint ctrl:1;
    uint alt:1;
    uint useLargeKeys:1;
    uint useOptiKeys:1;
    
    int pressedKey;

    KeyboardPicks *picks;

    int keyHeight;
    int defaultKeyWidth;
    int xoffs;

    int unicode;
    int qkeycode;
    int modifiers;

    int pressTid;
    bool pressed;

    QTimer *repeatTimer;
};

