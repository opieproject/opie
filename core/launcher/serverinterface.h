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
#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H


#include <qstring.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qtopia/applnk.h>
#include <qtopia/storage.h>


class ServerInterface {
public:
    virtual ~ServerInterface();

    virtual void createGUI() = 0;
    virtual void destroyGUI() = 0;

    // These notify of the application categories, like what is used in the current launcher tabs,
    // for example "Applications", "Games" etc.
    virtual void typeAdded( const QString& type, const QString& name, const QPixmap& pixmap, const QPixmap& bgPixmap ) = 0;
    virtual void typeRemoved( const QString& type ) = 0;

    // These functions notify when an application is added or removed and to which category it belongs
    virtual void applicationAdded( const QString& type, const AppLnk& doc ) = 0;
    virtual void applicationRemoved( const QString& type, const AppLnk& doc ) = 0;
    virtual void allApplicationsRemoved() = 0;
    // Useful for wait icons, or running application lists etc
    enum ApplicationState { Launching, Running, Terminated };
    virtual void applicationStateChanged( const QString& name, ApplicationState state ) = 0;
    static const AppLnkSet& appLnks();

    // These functions notify of adding, removal and changes of document links used in the documents tab for example
    virtual void aboutToAddBegin() {}
    virtual void aboutToAddEnd() {}
    virtual void documentAdded( const DocLnk& doc ) = 0;
    virtual void documentRemoved( const DocLnk& doc ) = 0;
    virtual void allDocumentsRemoved() = 0;
    virtual void documentChanged( const DocLnk& oldDoc, const DocLnk& newDoc ) = 0;

    virtual void storageChanged( const QList<FileSystem> & ) = 0;

    virtual void applicationScanningProgress( int percent ) = 0;
    virtual void documentScanningProgress( int percent ) = 0;

    // Tells the server if the GUI needs to be updated about documents or applnks
    virtual bool requiresApplications() const = 0;
    virtual bool requiresDocuments() const = 0;

    enum DockArea { Top, Bottom, Left, Right };
    static void dockWidget( QWidget *w, DockArea placement );
};


#endif // SERVER_INTERFACE_H
