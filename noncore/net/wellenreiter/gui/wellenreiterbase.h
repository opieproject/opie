/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef WELLENREITERBASE_H
#define WELLENREITERBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class OTabWidget;
class MLogWindow;
class MHexWindow;

class WellenreiterBase : public QWidget
{ 
    Q_OBJECT

public:
    WellenreiterBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~WellenreiterBase();

    OTabWidget* TabWidget;
    QWidget* ap;
    QListView* netview;
    MLogWindow* logwindow;
    MHexWindow* hexwindow;  
    QWidget* about;
    QLabel* PixmapLabel1_3_2;
    QLabel* TextLabel1_4_2;
    QPushButton* button;

protected:
    QVBoxLayout* WellenreiterBaseLayout;
    QVBoxLayout* apLayout;
    QGridLayout* aboutLayout;
    bool event( QEvent* );
};

#endif // WELLENREITERBASE_H
