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
#include <qwidget.h>
#include <qframe.h>
#include <qlist.h>
#include <qdict.h>

class QLabel;
class GraphData;
class Graph;
class GraphLegend;
class FileSystem;
class MountInfo;
class QVBoxLayout;


class StorageInfo : public QWidget
{
    Q_OBJECT
public:
    StorageInfo( QWidget *parent=0, const char *name=0 );

protected:
    void timerEvent(QTimerEvent*);

private:
    void updateMounts();
    QDict<MountInfo> disks;
    QList<QFrame> lines;
    QVBoxLayout *vb;
};

class MountInfo : public QWidget
{
    Q_OBJECT
public:
    MountInfo( const QString &path, const QString &ttl, QWidget *parent=0, const char *name=0 );
    ~MountInfo();

    void updateData();

private:
    QString title;
    FileSystem *fs;
    QLabel *totalSize;
    GraphData *data;
    Graph *graph;
    GraphLegend *legend;
};

class FileSystem
{
public:
    FileSystem( const QString &p );

    void update();

    const QString &path() const { return fspath; }
    long blockSize() const { return blkSize; }
    long totalBlocks() const { return totalBlks; }
    long availBlocks() const { return availBlks; }

private:
    QString fspath;
    long blkSize;
    long totalBlks;
    long availBlks;
};


