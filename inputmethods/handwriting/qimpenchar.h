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

#ifndef QIMPENCHAR_H_
#define QIMPENCHAR_H_

#include <qlist.h>
#include <qvaluelist.h>
#include <qcstring.h>
#include "qimpenstroke.h"

struct QIMPenSpecialKeys {
    int code;
    char *name;
};

extern const QIMPenSpecialKeys qimpen_specialKeys[];


class QIMPenChar
{
public:
    QIMPenChar();
    QIMPenChar( const QIMPenChar & );

    unsigned int character() const { return ch; }
    void setCharacter( unsigned int c ) { ch = c; }

    const QString &data() const { return d; }
    void setData( const QString &ba ) { d = ba; }

    QString name() const;
    bool isEmpty() const { return strokes.isEmpty(); }
    unsigned int strokeCount() const { return strokes.count(); }
    unsigned int strokeLength( int s ) const;
    void clear();
    int match( QIMPenChar *ch );
    const QIMPenStrokeList &penStrokes() { return strokes; }
    QPoint startingPoint() const { return strokes.getFirst()->startingPoint(); }
    QRect boundingRect();

    void setFlag( int f ) { flags |= f; }
    void clearFlag( int f ) { flags &= ~f; }
    bool testFlag( int f ) { return flags & f; }

    enum Flags { System=0x01, Deleted=0x02, CombineRight=0x04, Data=0x08 };
    // Correspond to codes in template files.  Do not change values.
    enum Mode { ModeBase=0x4000, Caps=0x4001, Shortcut=0x4002, CapsLock=0x4003,
		 Punctuation=0x4004, Symbol=0x4005, Extended=0x4006 };

    QIMPenChar &operator=( const QIMPenChar &s );

    void addStroke( QIMPenStroke * );

protected:
    unsigned int ch;
    QString d;
    Q_UINT8 flags;
    QIMPenStrokeList strokes;

    friend QDataStream &operator<< (QDataStream &, const QIMPenChar &);
    friend QDataStream &operator>> (QDataStream &, QIMPenChar &);
};

typedef QList<QIMPenChar> QIMPenCharList;
typedef QListIterator<QIMPenChar> QIMPenCharIterator;

QDataStream & operator<< (QDataStream & s, const QIMPenChar &ws);
QDataStream & operator>> (QDataStream & s, QIMPenChar &ws);

struct QIMPenCharMatch
{
    int error;
    QIMPenChar *penChar;

    bool operator>( const QIMPenCharMatch &m );
    bool operator<( const QIMPenCharMatch &m );
    bool operator<=( const QIMPenCharMatch &m );
};

typedef QValueList<QIMPenCharMatch> QIMPenCharMatchList;


class QIMPenCharSet
{
public:
    QIMPenCharSet();
    QIMPenCharSet( const QString &fn );

    bool isEmpty() const { return chars.isEmpty(); }
    unsigned int count() const { return chars.count(); }
    void clear() { chars.clear(); }

    void setDescription( const QString &d ) { desc = d; }
    QString description() const { return desc; }
    void setTitle( const QString &t ) { csTitle = t; }
    QString title() const { return csTitle; }

    QIMPenCharMatchList match( QIMPenChar *ch );
    void addChar( QIMPenChar *ch );
    void removeChar( QIMPenChar *ch );
    QIMPenChar *at( int i );

    unsigned maximumStrokes() const { return maxStrokes; }

    void up( QIMPenChar *ch );
    void down( QIMPenChar *ch );

    enum Domain { System, User };
    enum Type { Unknown=0x00, Lower=0x01, Upper=0x02, Combining=0x04,
		Numeric=0x08, Punctuation=0x10, Symbol=0x20, Shortcut=0x40 };

    const QIMPenCharList &characters() const { return chars; }

    void setType( Type t ) { csType = t; }
    Type type() const { return csType; }

    const QString &filename( Domain d ) const;
    void setFilename( const QString &fn, Domain d=System );
    bool load( const QString &fn, Domain d=System );
    bool save( Domain d=System );

protected:
    void markDeleted( uint ch );

protected:
    QString csTitle;
    QString desc;
    QString sysFilename;
    QString userFilename;
    Type csType;
    unsigned maxStrokes;
    QIMPenCharList chars;
    QIMPenCharMatchList matches;
};

typedef QList<QIMPenCharSet> QIMPenCharSetList;
typedef QListIterator<QIMPenCharSet> QIMPenCharSetIterator;

#endif
