/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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

#ifndef OTABINFO_H
#define OTABINFO_H

#include <qlist.h>
#include <qstring.h>

class QWidget;

/**
 * @class OTabInfo
 * @brief The OTabInfo class is used internally by OTabWidget to keep track
 *        of widgets added to the control.
 *
 * OTabInfo provides the following information about a widget added to an
 * OTabWidget control:
 *
 *  ID - integer tab bar ID
 *  Control - QWidget pointer to child widget
 *  Label - QString text label for OTabWidget selection control
 *  Icon - QString name of icon file
 */
class OTabInfo
{
public:
/**
 * @fn OTabInfo()
 * @brief Object constructor.
 *
 * @param parent Pointer to parent of this control.
 * @param name Name of control.
 * @param s Style of widget selection control.
 * @param p Position of the widget selection control.
 */
    OTabInfo() : i( -1 ), c( 0 ), p( 0 ), l( QString::null ) {}

/**
 * @fn OTabInfo( int id, QWidget *control, const QString &icon, const QString &label )
 * @brief Object constructor.
 *
 * @param id TabBar identifier for widget.
 * @param control QWidget pointer to widget.
 * @param icon QString name of icon file.
 * @param label QString text label for OTabWidget selection control.
 */
    OTabInfo( int id, QWidget *control, const QString &icon, const QString &label )
        : i( id ), c( control ), p( icon ), l( label ) {}

/**
 * @fn id()
 * @brief Returns TabBar ID.
 */
    int            id()      const { return i; }

/**
 * @fn label()
 * @brief Returns text label for widget.
 */
    const QString &label()   const { return l; }

/**
 * @fn control()
 * @brief Returns pointer to widget.
 */
    QWidget       *control() const { return c; }

/**
 * @fn icon()
 * @brief Returns name of icon file.
 */
    const QString &icon()    const { return p; }

private:
    int      i;
    QWidget *c;
    QString  p;
    QString  l;
};

/**
 * @class OTabInfoList
 * @brief A list of OTabInfo objects used by OTabWidget.
 */
typedef QList<OTabInfo> OTabInfoList;

#endif
