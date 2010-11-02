/*
                             This file is part of the Opie Project
               =.
             .=l.            Copyright (c) 2002-2004 The Opie Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef WINDOW_H
#define WINDOW_H

#include <qmainwindow.h>
#include "backuprestorebase.h"
#include <qmap.h>
#include <qlist.h>
#include <sys/stat.h>

class QListViewItem;
class QStringList;

class BackupAndRestore : public BackupAndRestoreBase
{

    Q_OBJECT

public:

    BackupAndRestore( QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~BackupAndRestore();

    static QString appName() { return QString::fromLatin1("backup"); }

private slots:
    void backup();
    void restore();
    void selectItem(QListViewItem *currentItem);
    void sourceDirChanged(int);
    void rescanFolder(QString directory, __dev_t origdev);
    void fileListUpdate();

    void addLocation();
    void removeLocation();
    void saveLocations();
    void selectLocation();

private:
    void scanForApplicationSettings();
    int getBackupFiles(QString &backupFiles, QListViewItem *parent);
    QString getExcludeFile();
    QMap<QString, QString> backupLocations;
    QList<QListViewItem> getAllItems(QListViewItem *item, QList<QListViewItem> &list);
    void refreshBackupLocations();
    void refreshLocations();
    __dev_t getDeviceOf( const QString &filename );

    void backupUserData();
    void backupRootFs();

    QListViewItem *systemSettings;
    QListViewItem *applicationSettings;
    QListViewItem *documents;
};

#endif

// backuprestore.h

