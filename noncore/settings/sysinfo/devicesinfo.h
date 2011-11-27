/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

/* OPIE */
#include <opie2/olistview.h>
#include <opie2/olayout.h>

/* QT */
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qstringlist.h>

namespace Opie { namespace Core { class OPcmciaSocket; class OInputDevice; }; };

//=================================================================================================
class DevicesView : public Opie::Ui::OListView
{
    Q_OBJECT
public:
    DevicesView( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~DevicesView();
public slots:
    void selectionChanged( QListViewItem* item );
};

//=================================================================================================
class DevicesInfo : public QWidget
{
    Q_OBJECT
public:
    DevicesInfo( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~DevicesInfo();

    void setDetailsWidget( QWidget* w = 0 );

private:
    Opie::Ui::OAutoBoxLayout* layout;
    Opie::Ui::OListView* view;
    QWidget* details;
    QWidgetStack* stack;
private slots:
};

//=================================================================================================
class Category : public Opie::Ui::OListViewItem
{
public:
    Category( DevicesView* parent, const QString& name );
    virtual ~Category();

    virtual void populate() = 0;
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
protected:
    QString readValue(const QString &file);
};

//=================================================================================================
class Device : public Opie::Ui::OListViewItem
{
public:
    Device( Category* parent, const QString& name );
    Device( Device* parent, const QString& name );
    ~Device();

    QWidget* devinfo;
    QWidget* details;

    virtual QWidget* detailsWidget();
};

//=================================================================================================
class CpuDevice : public Device
{
public:
    CpuDevice( Category* parent, const QString& name );
    ~CpuDevice();

    void addInfo( const QString& line );

//     virtual QWidget* detailsWidget();
};

//=================================================================================================
class InputDevice : public Device
{
public:
    InputDevice( Category* parent, const QString& name );
    ~InputDevice();

    void setInfo( const Opie::Core::OInputDevice* dev );

//     virtual QWidget* detailsWidget();
};

//=================================================================================================
class CardDevice : public Device
{
public:
    CardDevice( Category* parent, const QString& name );
    ~CardDevice();
    void setInfo( const Opie::Core::OPcmciaSocket* card );

//     virtual QWidget* detailsWidget();
};

//=================================================================================================
class UsbDevice : public Device
{
public:
    UsbDevice( Category* parent, const QString& name );
    UsbDevice( UsbDevice* parent, const QString& name );
    ~UsbDevice();
    void setInfo( const QStringList &params );

//     virtual QWidget* detailsWidget();
};

#endif
