
/*

               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 zecke <zecke@handhelds.org>
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

/*
 * @internal
 */
namespace {
  struct WidSize {
    QWidget *wid;
    int size;
  };

};

// forward declarations
class Config;

/**
 * This class is a specialised QPopupMenu. It'll display three different
 * font sizes. Small, Normal and large by adding widgets to the Menu
 * you can control the font size of these widgets
 * by using the save and restore method you can also apply font sizes
 * over two different runtimes
 *
 * <pre>
 * QTable* tbl = new QTable();
 * QList<QWidget> wid;
 * wid.append( tbl );
 * OFontMenu* menu = new OFontMenu(this, "Popup Menu", wid );
 * Config cfg("mycfg");
 * menu->restore( cfg );
 * </pre>
 *
 * @author Holger Freyther ( zecke@handhelds.org )
 * @version 0.1
 * @short PopupMenu which can control the size of Widgets
 * @see QPopupMenu
 */
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
    /**
     * this signal gets emitted when the font size gets changed
     * @param size The new size of font
     */
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





