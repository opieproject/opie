/*
 * todayconfig.h
 *
 * ---------------------
 *
 * begin       : Sun 10 17:20:00 CEST 2002
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef TODAYCONFIG_H
#define TODAYCONFIG_H

#include <qvariant.h>
#include <qdialog.h>
//class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QFrame;
class QLabel;
class QSpinBox;
class QTabWidget;
class QWidget;

class todayconfig : public QDialog {
    Q_OBJECT

public:
    todayconfig( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~todayconfig();

    QTabWidget* TabWidget3;
    QWidget* tab;
    QFrame* Frame8;
    QLabel* TextLabel4;
    QLabel* TextLabel5;
    QLabel* TextLabel2;
    QCheckBox* CheckBox3;
    QCheckBox* CheckBox2;
    QCheckBox* CheckBox1;
    QCheckBox* CheckBoxAuto;
    QSpinBox* SpinBox1;
    QLabel* TextLabel3;
    QWidget* tab_2;
    QFrame* Frame9;
    QLabel* TextLabel6;
    QSpinBox* SpinBox2;
    QWidget* tab_3;
    QFrame* Frame14;
    QLabel* TextLabel1;
    QSpinBox* SpinBox7;
    QLabel* TimeLabel;
    QSpinBox* SpinBoxTime;

};

#endif
