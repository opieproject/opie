/****************************************************************************
**
** Definition of QSqlManager class
**
** Created : 2000-11-03
**
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of the sql module of the Qt GUI Toolkit.
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
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
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

#ifndef QSQLMANAGER_P_H
#define QSQLMANAGER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "qglobal.h"
#include "qstring.h"
#include "qstringlist.h"
#include "qsql.h"
#include "qsqlerror.h"
#include "qsqlindex.h"
#include "qsqlcursor.h"
#endif // QT_H

#if !defined( QT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define QM_EXPORT_SQL
#else
#define QM_EXPORT_SQL Q_EXPORT
#endif

#ifndef QT_NO_SQL

class QSqlCursor;
class QSqlForm;
class QSqlCursorManagerPrivate;

class QM_EXPORT_SQL QSqlCursorManager
{
public:
    QSqlCursorManager();
    virtual ~QSqlCursorManager();

    virtual void setSort( const QSqlIndex& sort );
    virtual void setSort( const QStringList& sort );
    QStringList  sort() const;
    virtual void setFilter( const QString& filter );
    QString filter() const;
    virtual void setCursor( QSqlCursor* cursor, bool autoDelete = FALSE );
    QSqlCursor* cursor() const;

    virtual void setAutoDelete( bool enable );
    bool autoDelete() const;

    virtual bool refresh();
    virtual bool findBuffer( const QSqlIndex& idx, int atHint = 0 );

private:
    QSqlCursorManagerPrivate* d;
};

#ifndef QT_NO_SQL_FORM

class QSqlFormManagerPrivate;

class QM_EXPORT_SQL QSqlFormManager
{
public:
    QSqlFormManager();
    virtual ~QSqlFormManager();

    virtual void setForm( QSqlForm* form );
    QSqlForm* form();
    virtual void setRecord( QSqlRecord* record );
    QSqlRecord* record();

    virtual void clearValues();
    virtual void readFields();
    virtual void writeFields();

private:
    QSqlFormManagerPrivate* d;
};

#endif

class QWidget;
class QDataManagerPrivate;

class QM_EXPORT_SQL QDataManager
{
public:
    QDataManager();
    virtual ~QDataManager();

    virtual void setMode( QSql::Op m );
    QSql::Op mode() const;
    virtual void setAutoEdit( bool autoEdit );
    bool autoEdit() const;

    virtual void handleError( QWidget* parent, const QSqlError& error );
    virtual QSql::Confirm confirmEdit( QWidget* parent, QSql::Op m );
    virtual QSql::Confirm confirmCancel( QWidget* parent, QSql::Op m );

    virtual void setConfirmEdits( bool confirm );
    virtual void setConfirmInsert( bool confirm );
    virtual void setConfirmUpdate( bool confirm );
    virtual void setConfirmDelete( bool confirm );
    virtual void setConfirmCancels( bool confirm );

    bool confirmEdits() const;
    bool confirmInsert() const;
    bool confirmUpdate() const;
    bool confirmDelete() const;
    bool confirmCancels() const;

private:
    QDataManagerPrivate* d;
};


#endif
#endif
