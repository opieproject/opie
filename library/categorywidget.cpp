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

#include "categoryedit_p.h"
#include "categorywidget.h"

CategoryWidget::CategoryWidget( QWidget *parent, const char *name )
    : QVBox( parent, name )
{
    ce = new CategoryEdit( this, name );
}

CategoryWidget::CategoryWidget( const QArray<int> &vlRecs,
				const QString &appName, QWidget *parent,
				const char *name )
    : QVBox( parent, name )
{
    ce = new CategoryEdit( vlRecs, appName, appName, this, name );
}

CategoryWidget::CategoryWidget( const QArray<int> &vlRecs,
				const QString &appName, const QString &visibleName,
				QWidget *parent, const char *name )
    : QVBox( parent, name )
{
    ce = new CategoryEdit( vlRecs, appName, visibleName, this, name );
}

CategoryWidget::~CategoryWidget()
{
}

void CategoryWidget::setCategories( const QArray<int> &vlRecs,
				    const QString &appName )
{
    ce->setCategories( vlRecs, appName, appName );
}

void CategoryWidget::setCategories( const QArray<int> &vlRecs,
				    const QString &appName,
				    const QString &visibleName )
{
    ce->setCategories( vlRecs, appName, visibleName );
}

QArray<int> CategoryWidget::newCategories()
{
    return ce->newCategories();
}

void CategoryWidget::kludge()
{
    ce->kludge();
}
