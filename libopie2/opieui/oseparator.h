/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
                             Copyright (C) 1997 Michael Roth <mroth@wirlweb.de>
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

#ifndef OSEPARATOR_H
#define OSEPARATOR_H

#include <qframe.h>

namespace Opie {
namespace Ui   {

/**
 * Standard horizontal or vertical separator.
 *
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 * @author Michael Roth <mroth@wirlweb.de>
 * @version $Id: oseparator.h,v 1.2 2004-03-13 19:51:50 zecke Exp $
*/
class OSeparator : public QFrame
{
  Q_OBJECT
  Q_PROPERTY( int orientation READ orientation WRITE setOrientation )
 public:
  /**
   * Constructor.
   **/
  OSeparator(QWidget* parent=0, const char* name=0, WFlags f=0);
  /**
   * Constructor.
   *
   * @param orientation Set the orientation of the separator.
   * Possible values are HLine or Horizontal and VLine or Vertical.
   **/
  OSeparator(int orientation, QWidget* parent=0, const char* name=0, 
	     WFlags f=0);
  
  /**
   * Returns the orientation of the separator.
   *
   * Possible values are VLine and HLine.
   **/
  int orientation() const;
  
  /**
   * Set the orientation of the separator to @p orient
   *
   * Possible values are VLine and HLine.
   */
  void setOrientation(int orient);
  
  /**
   * The recommended height (width) for a horizontal (vertical) separator.
   **/
  virtual QSize sizeHint() const;

protected:
  virtual void drawFrame( QPainter * );

private:
  class OSeparatorPrivate* d;
};

}
}

#endif // OSEPARATOR_H
