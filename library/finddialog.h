/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
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


//
//   DO NOT ATTEMPT TO USE THIS CLASS
//

#ifndef __FINDDIALOG_H__
#define __FINDDIALOG_H__

#include <qdatetime.h>
#include <qdialog.h>

class FindWidget;

class FindDialogPrivate;
class FindDialog : public QDialog
{
    Q_OBJECT
public:

// WARNING: Do *NOT* define this yourself. The SL5xxx from SHARP does NOT
//      have this class.
#ifdef QTOPIA_INTERNAL_FD

    FindDialog( const QString &appName,
                QWidget *parent = 0, const char *name = 0, bool modal = TRUE );
    ~FindDialog();
#endif

    QString findText() const;
    void setUseDate( bool show );
    void setDate( const QDate &dt );

public slots:
    void slotNotFound();
    void slotWrapAround();

signals:
    void signalFindClicked( const QString &txt, bool caseSensitive,
                            bool backwards, int category );
    void signalFindClicked( const QString &txt, const QDate &dt,
			    bool caseSensitive, bool backwards, int category );

private:
    FindWidget *fw;
    FindDialogPrivate *d;
};

#endif
