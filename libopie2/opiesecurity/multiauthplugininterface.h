/**
 * \file multiauthplugininterface.h
 * \brief Main public interface to Opie multiauth authentication plugins.
 * 
 * This implementation was derived from the todolist plugin implementation.
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

#ifndef MULTIAUTH_PLUGIN_INTERFACE
#define MULTIAUTH_PLUGIN_INTERFACE

#include <qpe/qcom.h>

#include "multiauthconfigwidget.h"

class QString;
class QWidget;

#ifndef IID_MultiauthPluginInterface
/// Universally Unique Id of the interface (required by QCOM):
/// {AD5BE8B1-8421-447b-9AED-250BE1CDA49c}
#define IID_MultiauthPluginInterface QUuid(0xad5be8b1, 0x8421, 0x447b, 0x9a, 0xed, 0x25, 0x0b, 0xe1, 0xcd, 0xa4, 0x9c)
#endif

/// Base class for Opie multiauth plugins
/**
 * A MultiauthPluginObject is the base for all Multiauth Plugins.
 * A plugin author needs to inherit this class and implement
 * the pure virtual methods.
 *
 * \author Clement Seveillac (helped by Maximilian Reiß's Today framework)
 */
class MultiauthPluginObject {

    public:

        virtual ~MultiauthPluginObject() {};

        /// The name of the plugin
        /*
         * \return The plugin should return its name here
         */
        virtual QString pluginName() const = 0;

        /// Ask the plugin to launch one authentication attempt
        virtual int authenticate() = 0;

        /// Possible return codes for authenticate() function
        enum authResult { Success = 0,
            Failure = 1,
            Skip = 2 };

        /// Pointer to the (optional) plugin configuration widget
        virtual MultiauthConfigWidget * configWidget(QWidget * parent) = 0;

        /// (optional) plugin configuration tab icon
        /**
         * \return path to the image file (without the extension)
         * from $OPIEDIR/pics
         */
        virtual QString pixmapNameConfig() const = 0;

        /// Plugin icon, to be displayed in the plugin list configuration widget
        /**
         * \return path to the image file (without the extension)
         * from $OPIEDIR/pics
         */
        virtual  QString pixmapNameWidget() const = 0;

};

/// Interface for multiauth plugin classes.
/* 
 * This is part of the QCOM works. See example plugins how to do it right.
 * \see http://doc.trolltech.com/qtopia/html/pluginintro.html
 */
struct MultiauthPluginInterface : public QUnknownInterface {
    /// return the MultiauthPluginObject implementation
    virtual MultiauthPluginObject *plugin() = 0;
};

#endif
