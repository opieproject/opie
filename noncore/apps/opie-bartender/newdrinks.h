/****************************************************************************
** Created: Sat Jul 20 08:11:05 2002
**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#ifndef NEW_DRINK_H
#define NEW_DRINK_H

#include <qvariant.h>
#include <qwidget.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QMultiLineEdit;

class New_Drink : public QDialog
{ 
    Q_OBJECT

public:
    New_Drink( QWidget* parent = 0, const char* name = 0, bool modal=TRUE, WFlags fl = 0 );
    ~New_Drink();

    QLabel* TextLabel1;
    QLineEdit* LineEdit1;
    QLabel* TextLabel2;
    QMultiLineEdit* MultiLineEdit1;

protected:
    QGridLayout* Layout5;
    QHBoxLayout* Layout4;
    QString drinkName;
    
};

#endif // NEW_DRINK_H
