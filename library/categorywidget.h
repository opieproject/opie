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

#ifndef __CATEGORYWIDGET_H__
#define __CATEGORYWIDGET_H__

#include <qvbox.h>
#include <qarray.h>

class CategoryEdit;

class CategoryWidget : public QVBox
{
public:
    CategoryWidget( QWidget *parent = 0, const char *name = 0 );
    CategoryWidget( const QArray<int> &vlRecs, const QString &appName,
		    QWidget *parent = 0, const char *name = 0 );
    CategoryWidget( const QArray<int> &vlRecs, const QString &appName,
		    const QString &visibleName,
		    QWidget *parent = 0, const char *name = 0 );
    ~CategoryWidget();
    void setCategories( const QArray<int> &vlRecs, const QString &appName );
    void setCategories( const QArray<int> &vlRecs, const QString &appName,
			const QString &visibleName );
    QArray<int> newCategories();
    void kludge();

private:
    CategoryEdit *ce;
};


#endif
