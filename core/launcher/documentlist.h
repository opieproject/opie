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
#ifndef DOCUMENT_LIST_H
#define DOCUMENT_LIST_H


#include <qobject.h>
#include <qtopia/applnk.h>


class DocumentListPrivate;
class ServerInterface;

class DocumentList : public QObject {
    Q_OBJECT
public:
    DocumentList( ServerInterface *serverGui, bool scanDocs = TRUE,
		  QObject *parent = 0, const char *name = 0 );
    ~DocumentList();

    void linkChanged( QString link );
    void restoreDone();
    void storageChanged();
    void sendAllDocLinks();

    void reloadAppLnks();
    void reloadDocLnks();

// Orig DocList stuff
    void start();
    void pause();
    void resume();
    void rescan();

    static AppLnkSet *appLnkSet;

/*
    void resend();
signals:
    void added( const DocLnk& doc );
    void removed( const DocLnk& doc );
    void changed( const DocLnk& oldDoc, const DocLnk& newDoc );
    void allRemoved();
    void doneForNow();
private slots:
   void resendWorker();
*/
private slots:
    void startInitialScan();
protected:
    void timerEvent( QTimerEvent *te );
private:
    void add( const DocLnk& doc );
    DocumentListPrivate *d;

};


#endif

