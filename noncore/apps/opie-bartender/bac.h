/****************************************************************************
** Created: Sun Jul 21 18:59:50 2002
**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#ifndef BACDIALOG_H
#define BACDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLCDNumber;
class QLabel;
class QPushButton;
class QSpinBox;

class BacDialog : public QDialog
{ 
    Q_OBJECT

public:
    BacDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~BacDialog();

    QSpinBox* NumberSpinBox;
    QLabel* TextLabel1;
    QSpinBox* WeightSpinBox;
    QLabel* TextLabel2;
    QSpinBox* TimeSpinBox;
    QLabel* TextLabel3;
    QComboBox* GenderComboBox;
    QLabel* TextLabel4;
    QComboBox* TypeDrinkComboBox;
    QLabel* TextLabel1_2;
    QPushButton* PushButton1;
    QLCDNumber* LCDNumber1;
    QComboBox *weightUnitsCombo;
    QLabel *weightUnitsLabel;
protected:
    QVBoxLayout* Layout7;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout2;
    QHBoxLayout* Layout3;
    QHBoxLayout* Layout4;
    QHBoxLayout* Layout6;

protected slots:
    void calculate();
        
};

#endif // BACDIALOG_H
