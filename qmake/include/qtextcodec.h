/****************************************************************************
** $Id: qtextcodec.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of QTextCodec class
**
** Created : 981015
**
** Copyright (C) 1998-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QTEXTCODEC_H
#define QTEXTCODEC_H

#ifndef QT_H
#include "qstring.h"
#endif // QT_H

#ifndef QT_NO_TEXTCODEC

class QTextCodec;
class QIODevice;
class QFont;

class Q_EXPORT QTextEncoder {
public:
    virtual ~QTextEncoder();
    virtual QCString fromUnicode(const QString& uc, int& lenInOut) = 0;
};

class Q_EXPORT QTextDecoder {
public:
    virtual ~QTextDecoder();
    virtual QString toUnicode(const char* chars, int len) = 0;
};

class Q_EXPORT QTextCodec {
public:
    virtual ~QTextCodec();

#ifndef QT_NO_CODECS
    static QTextCodec* loadCharmap(QIODevice*);
    static QTextCodec* loadCharmapFile(QString filename);
#endif //QT_NO_CODECS
    static QTextCodec* codecForMib(int mib);
    static QTextCodec* codecForName(const char* hint, int accuracy=0);
    static QTextCodec* codecForContent(const char* chars, int len);
    static QTextCodec* codecForIndex(int i);
    static QTextCodec* codecForLocale();
    static void setCodecForLocale(QTextCodec *c);

    static void deleteAllCodecs();

    static const char* locale();

    virtual const char* name() const = 0;
    virtual const char* mimeName() const;
    virtual int mibEnum() const = 0;

    virtual QTextDecoder* makeDecoder() const;
    virtual QTextEncoder* makeEncoder() const;

    virtual QString toUnicode(const char* chars, int len) const;
    virtual QCString fromUnicode(const QString& uc, int& lenInOut) const;
    
    QCString fromUnicode(const QString& uc) const;
    QString toUnicode(const QByteArray&, int len) const;
    QString toUnicode(const QByteArray&) const;
    QString toUnicode(const QCString&, int len) const;
    QString toUnicode(const QCString&) const;
    QString toUnicode(const char* chars) const;
    virtual bool canEncode( QChar ) const;
    virtual bool canEncode( const QString& ) const;

    virtual int heuristicContentMatch(const char* chars, int len) const = 0;
    virtual int heuristicNameMatch(const char* hint) const;

    virtual QByteArray fromUnicode(const QString& uc, int from, int len) const;
    virtual unsigned short characterFromUnicode(const QString &str, int pos) const;

protected:
    QTextCodec();
    static int simpleHeuristicNameMatch(const char* name, const char* hint);

    friend class QFont;
};
#endif // QT_NO_TEXTCODEC
#endif // QTEXTCODEC_H
