
/*

               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 zekce <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
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




#ifndef ofontmenu_h
#define ofontmenu_h

#include <qpopupmenu.h>
#include <qlist.h>


namespace {
  struct WidSize {
    QWidget *wid;
    int size;
  };

};

// if i would be on kde this would be a KActionMenu...
class Config;
class OFontMenu : public QPopupMenu {
  Q_OBJECT
 public:
  OFontMenu(QWidget *parent, const char* name, const QList<QWidget> &list );
  void save(Config *cfg );
  void restore(Config *cfg );
  void setWidgets(const QList<QWidget> &list );
  void addWidget(QWidget *wid );
  void forceSize(QWidget *wid, int size );
  void removeWidget(QWidget *wid );
  const QList<QWidget> &widgets()const;

 signals:
  void fontChanged(int size );
 private:
  QList<QWidget> m_list;
  QList<WidSize> m_wids;
  int m_size;
  class OFontMenuPrivate;
  OFontMenuPrivate *d;
 private slots:
  virtual void slotSmall();
  virtual void slotMedium();
  virtual void slotLarge();
  void setFontSize(int size );
};

#endif





