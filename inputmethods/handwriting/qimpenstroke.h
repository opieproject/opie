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

#ifndef QIMPENSTROKE_H_
#define QIMPENSTROKE_H_

#include <qobject.h>
#include <qarray.h>
#include <qlist.h>

struct Q_PACKED QIMPenGlyphLink
{
    signed char dx;
    signed char dy;
};

class QIMPenStroke
{
public:
    QIMPenStroke();
    QIMPenStroke( const QIMPenStroke & );

    void clear();
    bool isEmpty() const { return links.isEmpty(); }
    unsigned int length() const { return links.count(); }
    unsigned int match( QIMPenStroke *st );
    const QArray<QIMPenGlyphLink> &chain() const { return links; }
    QPoint startingPoint() const { return startPoint; }
    void setStartingPoint( const QPoint &p ) { startPoint = p; }
    QRect boundingRect();

    QIMPenStroke &operator=( const QIMPenStroke &s );

    void beginInput( QPoint p );
    bool addPoint( QPoint p );
    void endInput();

    QArray<int> sig() { createTanSignature(); return tsig; } // for debugging

protected:
    void createSignatures();
    void createTanSignature();
    void createAngleSignature();
    void createDistSignature();
    int calcError( const QArray<int> &base, const QArray<int> &win,
                      int off, bool t );
    QArray<int> scale( const QArray<int> &s, unsigned count, bool t = FALSE );
    void internalAddPoint( QPoint p );
    QPoint calcCenter();
    int arcTan( int dy, int dx );
    QArray<int> createBase( const QArray<int> a, int e );
    void smooth( QArray<int> &);

protected:
    QPoint startPoint;
    QPoint lastPoint;
    QArray<QIMPenGlyphLink> links;
    QArray<int> tsig;
    QArray<int> asig;
    QArray<int> dsig;
    QRect bounding;

    friend QDataStream &operator<< (QDataStream &, const QIMPenStroke &);
    friend QDataStream &operator>> (QDataStream &, QIMPenStroke &);
};

typedef QList<QIMPenStroke> QIMPenStrokeList;
typedef QListIterator<QIMPenStroke> QIMPenStrokeIterator;

QDataStream & operator<< (QDataStream & s, const QIMPenStroke &ws);
QDataStream & operator>> (QDataStream & s, const QIMPenStroke &ws);

#endif

