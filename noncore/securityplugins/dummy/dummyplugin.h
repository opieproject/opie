/**
 * \file dummyplugin.h
 * \brief Standard Opie multiauth plugin definition
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

#ifndef DUMMY_PLUGIN_H
#define DUMMY_PLUGIN_H

#include <qobject.h>
#include <qstring.h>
#include <opie2/multiauthplugininterface.h>

/// Multi-authentication example plugin, implementing a trivial 'yes/no' verification.
/**
 * The plugin itself, implementing the main authenticate() function.
 */
class DummyPlugin : public QObject, public MultiauthPluginObject {
    
    Q_OBJECT
    
    public:
        int authenticate();
        MultiauthConfigWidget * configWidget(QWidget * parent);
        QString pixmapNameConfig() const;
        QString pixmapNameWidget() const;
        QString pluginName() const;
        
    signals:
        /// Signal carrying the result code of this plugin
        void emitCode(int resultCode);
        
    private slots:
        void success();
        void failure();
        void skip();
};

#endif
