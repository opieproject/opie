/****************************************************************************
**
** Definition of the QSqlExtension class
**
** Created : 2002-06-03
**
** Copyright (C) 2002 Trolltech AS.  All rights reserved.
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

#ifndef QSQLEXTENSION_P_H
#define QSQLEXTENSION_P_H

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
#include "qmap.h"
#include "qvaluevector.h"
#include "qstring.h"
#include "qvariant.h"
#include "qsql.h"
#endif // QT_H

#ifndef QT_NO_SQL

#if !defined( QT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define QM_EXPORT_SQL
#define QM_TEMPLATE_EXTERN_SQL
#else
#define QM_EXPORT_SQL Q_EXPORT
#define QM_TEMPLATE_EXTERN_SQL Q_TEMPLATE_EXTERN
#endif

struct Param {
    Param( const QVariant& v = QVariant(), QSql::ParameterType t = QSql::In ): value( v ), typ( t ) {}
    QVariant value;
    QSql::ParameterType typ;
    Q_DUMMY_COMPARISON_OPERATOR(Param)
};

struct Holder {
    Holder( const QString& hldr = QString::null, int pos = -1 ): holderName( hldr ), holderPos( pos ) {}
    bool operator==( const Holder& h ) const { return h.holderPos == holderPos && h.holderName == holderName; }
    bool operator!=( const Holder& h ) const { return h.holderPos != holderPos || h.holderName != holderName; }
    QString holderName;
    int	    holderPos;
};

#if defined(Q_TEMPLATEDLL)
QM_TEMPLATE_EXTERN_SQL template class QM_EXPORT_SQL QMap<QString,Param>;
QM_TEMPLATE_EXTERN_SQL template class QM_EXPORT_SQL QMap<int,QString>;
QM_TEMPLATE_EXTERN_SQL template class QM_EXPORT_SQL QValueVector<Holder>;
#endif

class QM_EXPORT_SQL QSqlExtension {
public:
    QSqlExtension();
    virtual ~QSqlExtension();
    virtual bool prepare( const QString& query );
    virtual bool exec();
    virtual void bindValue( const QString& holder, const QVariant& value, QSql::ParameterType = QSql::In );
    virtual void bindValue( int pos, const QVariant& value, QSql::ParameterType = QSql::In );
    virtual void addBindValue( const QVariant& value, QSql::ParameterType = QSql::In );
    virtual QVariant parameterValue( const QString& holder );
    virtual QVariant parameterValue( int pos );
    void clearValues();
    void clearIndex();

    enum BindMethod { BindByPosition, BindByName };
    BindMethod bindMethod();
    BindMethod bindm;
    int bindCount;

    QMap<int, QString> index;
    typedef QMap<QString, Param> ValueMap;
    ValueMap values;

    // convenience container for QSqlQuery
    // to map holders <-> positions
    typedef QValueVector<Holder> HolderVector;
    HolderVector holders;
};

class QM_EXPORT_SQL QSqlDriverExtension
{
public:
    QSqlDriverExtension();
    virtual ~QSqlDriverExtension();
    virtual bool isOpen() const = 0;
};
#endif
#endif
