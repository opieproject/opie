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
#include <qscrollview.h>

class QComboBox;

class UniScrollview : public QScrollView {
    Q_OBJECT
public:
    UniScrollview(QWidget* parent=0, const char* name=0, int f=0);
    int cellSize() const { return cellsize; }

signals:
    void key( ushort unicode, ushort scancode, ushort modifiers, bool, bool );

public slots:
    void scrollTo( int unicode );

protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent*);
    void drawContents( QPainter *, int cx, int cy, int cw, int ch ) ;

private:
    int cellsize;
    QFont smallFont;
    int xoff;
};


class UniKeyboard : public QFrame
{
    Q_OBJECT
public:
    UniKeyboard(QWidget* parent=0, const char* name=0, int f=0);
    ~UniKeyboard();

    void resetState();

    QSize sizeHint() const;

signals:
    void key( ushort, ushort, ushort, bool, bool );

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void handleCombo( int );
    void svMove( int, int );

private:
    UniScrollview *sv;
    QComboBox *cb;
    int currentBlock;
    int* cbmap;
};



