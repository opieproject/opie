/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef OVERSATILEVIEWDEMO_H
#define OVERSATILEVIEWDEMO_H

#include <qvbox.h>

namespace Opie {
namespace Ui   {
class OVersatileView;
class OVersatileViewItem;
}
}

class OVersatileViewDemo: public QVBox
{
  Q_OBJECT
  
  public:
    OVersatileViewDemo( QWidget* parent=0, const char* name=0, WFlags f=0 );
    virtual ~OVersatileViewDemo();

  public slots:
    void selectionChanged();
    void selectionChanged( Opie::Ui::OVersatileViewItem * );
    void currentChanged( Opie::Ui::OVersatileViewItem * );
    void clicked( Opie::Ui::OVersatileViewItem * );
    void pressed( OPie::Ui::OVersatileViewItem * );

    void doubleClicked( Opie::Ui::OVersatileViewItem *item );
    void returnPressed( Opie::Ui::OVersatileViewItem *item );
  
    void onItem( Opie::Ui::OVersatileViewItem *item );
    void onViewport();

    void expanded( Opie::Ui::OVersatileViewItem *item );
    void collapsed( Opie::Ui::OVersatileViewItem *item );
    
    void moved();

    void contextMenuRequested( Opie::Ui::OVersatileViewItem *item, const QPoint&, int col );
    
  private:
    Opie::Ui::OVersatileView* vv;

};

#endif
