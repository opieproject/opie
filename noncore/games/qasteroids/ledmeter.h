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
**********************************************************************//*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#ifndef __LEDMETER_H__
#define __LEDMETER_H__

#include <qframe.h>
#include <qlist.h>

#define QPtrList QList

class KALedMeter : public QFrame
{
    Q_OBJECT
public:
    KALedMeter( QWidget *parent );

    int range() const { return mRange; }
    void setRange( int r );

    int count() const { return mCount; }
    void setCount( int c );

    int value () const { return mValue; }

    void addColorRange( int pc, const QColor &c );

public slots:
    void setValue( int v );

protected:
    virtual void resizeEvent( QResizeEvent * );
    virtual void drawContents( QPainter * );
    void calcColorRanges();

protected:
    struct ColorRange
    {
	int mPc;
	int mValue;
	QColor mColor;
    };

    int mRange;
    int mCount;
    int mCurrentCount;
    int mValue;
    QPtrList<ColorRange> mCRanges;
};

#endif
