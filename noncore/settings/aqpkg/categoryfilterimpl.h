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

#include <qdialog.h>

class QListBox;

/**
  *@author Andy Qua
  */

class CategoryFilterImpl : public QDialog
{
   Q_OBJECT
public: 
    CategoryFilterImpl(const QString &categories, const QString &selectedCategories, QWidget *parent=0, const char *name=0);
    ~CategoryFilterImpl();

    QString getSelectedFilter();

private:
    QListBox *lstCategories;

};

#endif
