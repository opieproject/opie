/**
 * \file dummypluginimpl.h
 * \brief Standard Opie multiauth plugin interface definition
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

#ifndef DUMMY_PLUGIN_IMPL_H
#define DUMMY_PLUGIN_IMPL_H

#include "dummyplugin.h"

/// Standard multiauth plugin class
class DummyPluginImpl : public MultiauthPluginInterface{

    public:
        DummyPluginImpl();
        virtual ~DummyPluginImpl();

        QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
        /// defines standard addRef() and release() functions
        Q_REFCOUNT;

        virtual MultiauthPluginObject *plugin();

    private:
        /// the plugin itself
        DummyPlugin *dummyPlugin;
        ulong ref;
};

#endif
