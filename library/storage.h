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
#ifndef __storage_h__
#define __storage_h__

#include <qobject.h>
#include <qlist.h>

class FileSystem;
#ifndef QT_NO_COP
class QCopChannel;
#endif

class StorageInfo : public QObject
{
    Q_OBJECT
public:
    StorageInfo( QObject *parent=0 );

    const QList<FileSystem> &fileSystems() const { return mFileSystems; }
    const FileSystem *fileSystemOf( const QString &filename );

signals:
    void disksChanged();

public slots:
    void update();

private slots:
    void cardMessage( const QCString& msg, const QByteArray& data );
private:
    QList<FileSystem> mFileSystems;
#ifndef QT_NO_COP
    QCopChannel *channel;
#endif
};

class FileSystem
{
public:
    const QString &disk() const { return fsdisk; }
    const QString &path() const { return fspath; }
    const QString &name() const { return humanname; }
    const QString &options() const { return opts; }
    long blockSize() const { return blkSize; }
    long totalBlocks() const { return totalBlks; }
    long availBlocks() const { return availBlks; }
    bool isRemovable() const { return removable; }
    bool isWritable() const { return opts.contains("rw"); }

private:
    friend class StorageInfo;
    FileSystem( const QString &disk, const QString &path, const QString &humanname, bool rem, const QString &opts );
    void update();

    QString fsdisk;
    QString fspath;
    QString humanname;
    long blkSize;
    long totalBlks;
    long availBlks;
    bool removable;
    QString opts;
};


#endif
