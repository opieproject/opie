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

#ifndef __FINDWIDGET_H__
#define __FINDWIDGET_H__

#include "findwidgetbase_p.h"

#include <qdatetime.h>

class Categories;
class DateBookMonth;

class FindWidgetPrivate;
class FindWidget : public FindWidgetBase
{
    Q_OBJECT
public:
    FindWidget( const QString &appName,
		QWidget *parent = 0, const char *name = 0 );
    ~FindWidget();

    QString findText() const;
    void setUseDate( bool show );
    void setDate( const QDate &dt );

public slots:
    void slotNotFound();
    void slotWrapAround();
    void slotDateChanged( int year, int month, int day );

signals:
    void signalFindClicked( const QString &txt, bool caseSensitive,
			    bool backwards, int category );
    void signalFindClicked( const QString &txt, const QDate &dt,
			    bool caseSensitive, bool backwards, int category );

private slots:
    void slotFindClicked();

private:
    QString mStrApp;
    Categories *mpCat;
    DateBookMonth *dtPicker;
    QDate mDate;
};

#endif
