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

#ifndef OVERSATILEVIEWITEM_H
#define OVERSATILEVIEWITEM_H

/* QT */

#include <qiconview.h>

/* OPIE */

#include <opie2/olistview.h>

namespace Opie {
namespace Ui   {

class OVersatileView;

class OVersatileViewItem : public OListViewItem, public QIconViewItem
{
  public:
    OVersatileViewItem( OVersatileView * parent );
    
    OVersatileViewItem( OVersatileView * parent, OVersatileViewItem * after );
    
    OVersatileViewItem( OVersatileViewItem * parent, OVersatileViewItem * after );
    
    OVersatileViewItem( OVersatileView * parent, QString,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null );

    OVersatileViewItem( OVersatileViewItem * parent, QString,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null );

    OVersatileViewItem( OVersatileView * parent, OVersatileViewItem * after, QString,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null );

    OVersatileViewItem( OVersatileViewItem * parent, OVersatileViewItem * after, QString,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null, QString = QString::null,
                   QString = QString::null );
    
    virtual ~OVersatileViewItem();
    
    OVersatileView* versatileView() const;

    // TODO: Implement the remaining constructors from QIconView

    /*  OIconViewItem( QIconView *parent, const QString &text, const QPixmap &icon );
        OIconViewItem( QIconView *parent, QIconViewItem *after, const QString &text, const QPixmap &icon );
    */

    virtual void setRenameEnabled( bool );
    
    // TODO: Implement the remaining method multiplexers
  
  private:
    OVersatileView* _versatileview;
        
  private:
    void init();

};

}
}
#endif
