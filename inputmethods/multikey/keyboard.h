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
#include <qmap.h>
#include "../pickboard/pickboardcfg.h"
#include "../pickboard/pickboardpicks.h"
#include "configdlg.h"

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


class Keys {
public:

    Keys();
    Keys(const char * filename);
    ~Keys();
    ushort  uni(const int row, const int col);
    int     qcode(const int row, const int col);
    int     width(const int row, const int col);
    bool    pressed(const int row, const int col);
    bool    *pressedPtr(const int row, const int col);
    ushort  shift(const ushort);
    QPixmap *pix(const int row, const int col);
    int     numKeys(const int row);
    void    setKeysFromFile(const char *filename);
    void    setKey(const int row, const int qcode, const ushort unicode, 
                    const int width, QPixmap *pix);
    void    setPressed(const int row, const int col, const bool pressed);
    QString lang;
    QString label;

private:

    typedef struct Key {
	    int qcode; // are qt key codes just unicode values?
	    ushort unicode;
	    int width;  // not pixels but relative key width. normal key is 2

        // only needed for keys like ctrl that can have multiple keys pressed at once
        bool *pressed; 
	    QPixmap *pix;
    };

    QList<Key> keys[6];
    QMap<ushort,ushort> shiftMap;

};

class Keyboard : public QFrame
{
    Q_OBJECT
public:
    Keyboard( QWidget* parent=0, const char* name=0, WFlags f=0 );
    ~Keyboard();

    void resetState();

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent* e);
    //void timerEvent(QTimerEvent* e);
    void drawKeyboard( QPainter &p, int row = -1, int col = -1);

    QSize sizeHint() const;

signals:
    void key( ushort scancode, ushort unicode, ushort modifiers, bool, bool );

private slots:
    void repeat();
    void togglePickboard(bool on_off);
    void setMapToDefault();
    void setMapToFile(QString map);

    // used to redraw keyboard after edited colors
    void reloadKeyboard();
    
private:
    int getKey( int &w, int j = -1 );
    void clearHighlight();

    bool *shift;
    bool *lock;
    bool *ctrl;
    bool *alt;
    uint useLargeKeys:1;
    uint usePicks:1;
    
    int pressedKeyRow;
    int pressedKeyCol;

    KeyboardPicks *picks;

    int keyHeight;
    int defaultKeyWidth;
    int xoffs;

    int unicode;
    int qkeycode;
    int modifiers;

    int pressTid;
    bool pressed;

    Keys *keys;

    /* for korean input */
    ushort schar, mchar, echar;
    ushort parseKoreanInput(ushort c);
    ushort combineKoreanChars(const ushort s, const ushort m, const ushort e);
    ushort constoe(const ushort c);

    QTimer *repeatTimer;

    /* colors */
    void loadKeyboardColors();
    QColor keycolor;
    QColor keycolor_pressed;
    QColor keycolor_lines;
    QColor textcolor;

    ConfigDlg *configdlg;
};


