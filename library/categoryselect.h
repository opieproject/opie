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

#ifndef __CATEGORYCOMBO_H__
#define __CATEGORYCOMBO_H__

#include <qcombobox.h>
#include <qhbox.h>
#include <qstring.h>
#include <qarray.h>

extern QString categoryFileName();

class QToolButton;

class CategoryComboPrivate;
class CategoryCombo : public QComboBox
{
    Q_OBJECT

public:
    CategoryCombo( QWidget *parent, const char* name = 0 );
    ~CategoryCombo();

    int currentCategory() const;
    void setCurrentCategory( int id );
    // depreciated.
    void initCombo( const QArray<int> &recCats, const QString &appName );
    void initCombo( const QArray<int> &recCats, const QString &appName,
		    const QString &visibleName /* = appName */ );

signals:
    void sigCatChanged( int newUid );

private slots:
    void slotValueChanged( int );

private:
    void setCurrentText( const QString &str );
    CategoryComboPrivate *d;
};

#endif

class CategorySelectPrivate;
class CategorySelect : public QHBox
{
    Q_OBJECT
public:
    // we need two constructors, the first gets around designer limitations
    CategorySelect( QWidget *parent = 0, const char *name = 0 );

    CategorySelect( const QArray<int> &vlCats, const QString &appName,
		    QWidget *parent = 0, const char *name = 0 );
    CategorySelect( const QArray<int> &vlCats, const QString &appName,
		    const QString &visibleName, QWidget *parent = 0,
		    const char *name = 0 );
    ~CategorySelect();

    const QArray<int> &currentCategories() const;
    int currentCategory() const;
    void setCurrentCategory( int newCatUid );
    // pretty much if you don't set it the constructor, you need to
    // call it here ASAP!
    // however this call is depreciated...
    void setCategories( const QArray<int> &vlCats, const QString &appName );
    // use this one instead (for translating )
    void setCategories( const QArray<int> &vlCats, const QString &appName,
			const QString &visibleName );

    // these were added for find dialog.
    void setRemoveCategoryEdit( bool remove );
    void setAllCategories( bool add );

signals:
    void signalSelected( int );

private slots:
    void slotDialog();
    void slotNewCat( int id );

private:
    void init();
    QString mStrAppName;
    CategoryCombo *cmbCat;
    QToolButton *cmdCat;
    CategorySelectPrivate *d;
};
