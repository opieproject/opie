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

#ifndef __CATEGORYEDIT_H__
#define __CATEGORYEDIT_H__

#include "categoryeditbase_p.h"
#include <qarray.h>

class CategoryEditPrivate;

class CategoryEdit : public CategoryEditBase
{
    Q_OBJECT

public:
    CategoryEdit( QWidget *parent = 0, const char *name = 0 );
    CategoryEdit( const QArray<int> &vlRecs, const QString &appName,
		  const QString &visibleName,
		  QWidget *parent = 0, const char *name = 0 );
    ~CategoryEdit();
    void setCategories( const QArray<int> &vlRecs,
			const QString &appName, const QString &visibleName );
    QArray<int> newCategories();
    void kludge();

protected:
    void accept();

protected slots:
    void slotAdd();
    void slotRemove();
    void slotSetText( QListViewItem *selected );
    void slotSetGlobal( bool isChecked );
    void slotTextChanged( const QString &strNew );

private:
    CategoryEditPrivate *d;
};

QString categoryFileName();

#endif
