/*
 * todaybase.h
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : harlekin@handhelds.org
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

#ifndef TODAYBASE_H
#define TODAYBASE_H

#include <qlayout.h>
#include <opie/oclickablelabel.h>

class QVBox;
class QGridLayout;
class QFrame;
class QLabel;
class QPushButton;
class QBoxLayout;


class TodayBase : public QWidget
{
    Q_OBJECT

public:
    TodayBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TodayBase();

    QFrame* Frame4;
    QPushButton* DatesButton;
    QVBoxLayout* layout;
    OClickableLabel* OwnerField;
    QLabel* Frame;
    QLabel* DateLabel;
    QFrame* Frame15;
    OClickableLabel* ConfigButton;

protected:


};

#endif
