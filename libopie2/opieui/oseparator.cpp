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

/* OPIE */

#include <opie2/odebug.h>
#include <opie2/oseparator.h>

/* QT */

using namespace Opie::Core;
using namespace Opie::Ui;

OSeparator::OSeparator(QWidget* parent, const char* name, WFlags f)
   : QFrame(parent, name, f)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation( HLine );
}



OSeparator::OSeparator(int orientation, QWidget* parent, const char* name, WFlags f)
   : QFrame(parent, name, f)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation( orientation );
}



void OSeparator::setOrientation(int orientation)
{
   switch(orientation)
   {
      case Vertical:
      case VLine:
         setFrameStyle( QFrame::VLine | QFrame::Sunken );
         setMinimumSize(2, 0);
         break;
      
      default:
         owarn << "OSeparator::setOrientation(): invalid orientation, using default orientation HLine" << oendl;
         
      case Horizontal:
      case HLine:
         setFrameStyle( QFrame::HLine | QFrame::Sunken );
         setMinimumSize(0, 2);
         break;
   }
}



int OSeparator::orientation() const
{
   if ( frameStyle() & VLine )
      return VLine;
   
   if ( frameStyle() & HLine )
      return HLine;
   
   return 0;
}

void OSeparator::drawFrame(QPainter *p)
{
   QPoint	p1, p2;
   QRect	r     = frameRect();
   const QColorGroup & g = colorGroup();

   if ( frameStyle() & HLine ) {
      p1 = QPoint( r.x(), r.height()/2 );
      p2 = QPoint( r.x()+r.width(), p1.y() );
   }
   else {
      p1 = QPoint( r.x()+r.width()/2, 0 );
      p2 = QPoint( p1.x(), r.height() );
   }

#if QT_VERSION < 300
   style().drawSeparator( p, p1.x(), p1.y(), p2.x(), p2.y(), g, true, 1, midLineWidth() );
#else
   QStyleOption opt( lineWidth(), midLineWidth() );
   style().drawPrimitive( QStyle::PE_Separator, p, QRect( p1, p2 ), g, QStyle::Style_Sunken, opt );
#endif
}


QSize OSeparator::sizeHint() const
{
   if ( frameStyle() & VLine )
      return QSize(2, 0);
   
   if ( frameStyle() & HLine )
      return QSize(0, 2);
   
   return QSize(-1, -1);
}
