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
#ifndef SHOW_DRINK_H
#define SHOW_DRINK_H

#include <qvariant.h>
#include <qwidget.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QMultiLineEdit;
class QString;
class QPushButton;

class Show_Drink : public QDialog
{ 
    Q_OBJECT

public:
    Show_Drink( QWidget* parent = 0, const char* name = 0, bool modal=TRUE, WFlags fl = 0 );
    ~Show_Drink();

    QMultiLineEdit* MultiLineEdit1;
public slots:
    void slotEditDrink();
protected:
    QGridLayout* Layout5;
    QString drinkName;
    QPushButton *editDrinkButton;
};

#endif // SHOW_DRINK_H
