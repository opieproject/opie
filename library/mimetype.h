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
#ifndef MIMETYPE_H
#define MIMETYPE_H

#include <qstring.h>
#include <qpixmap.h>

class AppLnk;
class DocLnk;
class QStringList;
class MimeTypeData;
class AppLnkSet;

class MimeType
{
public:
    MimeType( const QString& ext_or_id );
    MimeType( const DocLnk& );

    QString id() const;
    QString description() const;
    QPixmap pixmap() const;
    QPixmap bigPixmap() const;

// DON'T define this yourself!
#ifdef QTOPIA_INTERNAL_MIMEEXT
    QString extension() const;
#endif

    const AppLnk* application() const;

    static QString appsFolderName();
    static void updateApplications();

    // These shouldn't be needed
    static void clear();
    static void registerApp( const AppLnk& );

private:
    static void updateApplications(AppLnkSet* folder);
    static void loadExtensions();
    static void loadExtensions(const QString&);
    void init( const QString& ext_or_id );
    class Dict;
    static Dict* d;
    static Dict& dict();
    static MimeTypeData* data(const QString& id);
    QString i;
};

#endif
