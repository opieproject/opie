/***************************************************************************
                          categoryfilterimpl.h  -  description
                             -------------------
    begin                : Sun Nov 17 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CATEGORYFILTERIMPL_H
#define CATEGORYFILTERIMPL_H

#include <qwidget.h>
#include <qstring.h>

#include "categoryfilter.h"

/**
  *@author Andy Qua
  */

class CategoryFilterImpl : public CategoryFilterDlg
{
   Q_OBJECT
public: 
    CategoryFilterImpl(const QString &categories, const QString &selectedCategories, QWidget *parent=0, const char *name=0);
    ~CategoryFilterImpl();

    QString getSelectedFilter();
};

#endif
