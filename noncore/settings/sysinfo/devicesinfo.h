/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :          the terms of the GNU General Public
.="- .-=="i,     .._        License as published by the Free Software
- .   .-<_>     .<>         Foundation; version 2 of the License.
    ._= =}       :
   .%`+i>       _;_.
   .i_,=:_.      -<s.       This program is distributed in the hope that
    +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
   : ..    .:,     . . .    without even the implied warranty of
   =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;     General Public License for more
++=   -.     .`     .:      details.
:     =  ...= . :.=-
-.   .:....=;==+<;          You should have received a copy of the GNU
 -_. . .   )=.  =           General Public License along with
   --        :-=`           this application; see the file LICENSE.GPL.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

#ifndef DEVICESINFO_H
#define DEVICESINFO_H

#include <qwidget.h>
#include <opie2/olistview.h>
using namespace Opie::Ui;

//=================================================================================================
class DevicesView : public OListView
{
    Q_OBJECT
public:
    DevicesView( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~DevicesView();
};

//=================================================================================================
class DevicesInfo : public QWidget
{
    Q_OBJECT
public:
    DevicesInfo( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~DevicesInfo();

private:
    OListView* view;

private slots:
};

//=================================================================================================
class Category : public OListViewItem
{
public:
    Category( DevicesView* parent, const QString& name );
    virtual ~Category();

    virtual void populate() = 0;
};

//=================================================================================================
class Device : public OListViewItem
{
public:
    Device( Category* parent, const QString& name );
    ~Device();
};

//=================================================================================================
class CpuCategory : public Category
{
public:
    CpuCategory( DevicesView* parent );
    virtual ~CpuCategory();

    virtual void populate();
};

//=================================================================================================
class InputCategory : public Category
{
public:
    InputCategory( DevicesView* parent );
    virtual ~InputCategory();

    virtual void populate();
};

//=================================================================================================
class CardsCategory : public Category
{
public:
    CardsCategory( DevicesView* parent );
    virtual ~CardsCategory();

    virtual void populate();
};

//=================================================================================================
class UsbCategory : public Category
{
public:
    UsbCategory( DevicesView* parent );
    virtual ~UsbCategory();

    virtual void populate();
};


#endif
