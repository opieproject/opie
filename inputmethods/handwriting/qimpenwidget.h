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

#include <qwidget.h>
#include <qlist.h>
#include "qimpenchar.h"

class QIMPenWidget : public QWidget
{
    Q_OBJECT
public:
    QIMPenWidget( QWidget *parent );

    void clear();
    void greyStroke();
    void setReadOnly( bool r ) { readOnly = r; }

    void insertCharSet( QIMPenCharSet *cs, int stretch=1, int pos=-1 );
    void removeCharSet( int );
    void changeCharSet( QIMPenCharSet *cs, int pos );
    void clearCharSets();
    void showCharacter( QIMPenChar *, int speed = 10 );
    virtual QSize sizeHint();

public slots:
    void removeStroke();

signals:
    void changeCharSet( QIMPenCharSet *cs );
    void changeCharSet( int );
    void beginStroke();
    void stroke( QIMPenStroke *ch );

protected slots:
    void timeout();

protected:
    enum Mode { Waiting, Input, Output };
    bool selectSet( QPoint );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void paintEvent( QPaintEvent *e );
    virtual void resizeEvent( QResizeEvent *e );

    struct CharSetEntry {
	QIMPenCharSet *cs;
	int stretch;
    };
    typedef QList<CharSetEntry> CharSetEntryList;
    typedef QListIterator<CharSetEntry> CharSetEntryIterator;

protected:
    Mode mode;
    bool autoHide;
    bool readOnly;
    QPoint lastPoint;
    unsigned pointIndex;
    int strokeIndex;
    int currCharSet;
    QTimer *timer;
    QColor strokeColor;
    QRect dirtyRect;
    QIMPenChar *outputChar;
    QIMPenStroke *outputStroke;
    QIMPenStroke *inputStroke;
    QIMPenStrokeList strokes;
    CharSetEntryList charSets;
    int totalStretch;
};

