/****************************************************************************
** $Id: qfile.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of QFile class
**
** Created : 930831
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
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

#ifndef QFILE_H
#define QFILE_H

#ifndef QT_H
#include "qiodevice.h"
#include "qstring.h"
#include <stdio.h>
#endif // QT_H

class QDir;


class Q_EXPORT QFile : public QIODevice			// file I/O device class
{
public:
    QFile();
    QFile( const QString &name );
   ~QFile();

    QString	name()	const;
    void	setName( const QString &name );

    typedef QCString (*EncoderFn)( const QString &fileName );
    typedef QString (*DecoderFn)( const QCString &localfileName );
    static QCString encodeName( const QString &fileName );
    static QString decodeName( const QCString &localFileName );
    static void setEncodingFunction( EncoderFn );
    static void setDecodingFunction( DecoderFn );

    bool	exists()   const;
    static bool exists( const QString &fileName );

    bool	remove();
    static bool remove( const QString &fileName );

    bool	open( int );
    bool	open( int, FILE * );
    bool	open( int, int );
    void	close();
    void	flush();

    Offset	size() const;
    Offset	at() const;
    bool	at( Offset );
    bool	atEnd() const;

    Q_LONG	readBlock( char *data, Q_ULONG len );
    Q_LONG	writeBlock( const char *data, Q_ULONG len );
    Q_LONG	writeBlock( const QByteArray& data )
		      { return QIODevice::writeBlock(data); }
    Q_LONG	readLine( char *data, Q_ULONG maxlen );
    Q_LONG	readLine( QString &, Q_ULONG maxlen );

    int		getch();
    int		putch( int );
    int		ungetch( int );

    int		handle() const;

protected:
    QString	fn;
    FILE       *fh;
    int		fd;
    Offset	length;
    bool	ext_f;
    void * 	d;

private:
    void	init();
    QCString	ungetchBuffer;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QFile( const QFile & );
    QFile &operator=( const QFile & );
#endif
};


inline QString QFile::name() const
{ return fn; }

inline QIODevice::Offset QFile::at() const
{ return ioIndex; }


#endif // QFILE_H
