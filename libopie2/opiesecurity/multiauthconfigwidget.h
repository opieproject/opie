/**
 * \file multiauthconfigwidget.h
 * \brief Defines the Opie multiauth configuration widget interface.
 * \author Clément Séveillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-        
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB. 
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef MULTIAUTHCONFIGWIDGET_H
#define MULTIAUTHCONFIGWIDGET_H

#include <qwidget.h>
#include <qobject.h>

/// Base class of all Opie multiauth config widgets
/**
 * A base class for all Opie Multiauth Config Widgets.
 * This will let a Opie multiauth plugin to add the possibility of configuration.
 * Plugins need to inherit from this class and need to implement
 * the pure virtual method to control configuration.
 * The Plugin should read its configuration during creation of the Widget
 *
 * \author Clement Seveillac (helped by Maximilian Reiß's Today framework)
 */
class MultiauthConfigWidget : public QWidget {

    Q_OBJECT

    public:

        /// standard widget constructor
        MultiauthConfigWidget( QWidget *parent,  const char *name ) : QWidget( parent,  name ) {}
        virtual ~MultiauthConfigWidget() {}

        /// Method to reimplement, supposed to save the plugin configuration
        /**
         * Plugins need to reimplement this in the config widget
         * Used when the config dialog is closed to write config stuff
         */
        virtual void  writeConfig() = 0;
};

#endif // MULTIAUTHCONFIGWIDGET_H
