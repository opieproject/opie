/**********************************************************************
** Copyright (C) 2000-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
**
** A copy of the GNU GPL license version 2 is included in this package as
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef LOCATIONCOMBO_H
#define LOCATIONCOMBO_H

#include <qpe/qpeglobal.h>

#ifdef Q_WS_QWS

#include <qstringlist.h>
#include <qcombobox.h>

class AppLnk;
class QListViewItem;
class DocLnk;
class FileSystem;
class StorageInfo;


class LocationComboPrivate;

class LocationCombo : public QComboBox
{
    Q_OBJECT
public:
    LocationCombo( QWidget *parent, const char *name=0 );
    LocationCombo( const AppLnk * lnk, QWidget *parent, const char *name=0 );

    ~LocationCombo();

    void setLocation( const QString& path ); // qtopia 2
    void setLocation( const AppLnk * );

    QString installationPath() const;
    QString documentPath() const;
    const FileSystem *fileSystem() const;

    bool isChanged() const;

signals:
    void newPath();

private slots:
    void updatePaths();

private:
    void setupCombo();
    QStringList locations;
    StorageInfo *storage;
    LocationComboPrivate *d;
};


#endif // QWS
#endif // LNKPROPERTIES_H
