/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
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

#include <opie2/oapplication.h>
#include <opie2/oconfig.h>

#include <signal.h>
#include <stdio.h>

OApplication* OApplication::_instance = 0;

/**************************************************************************************************/
/* OApplicationPrivate
/**************************************************************************************************/

class OApplicationPrivate
{
  public:
    OApplicationPrivate() {};
    ~OApplicationPrivate() {};
};

/**************************************************************************************************/
/* OApplication
/**************************************************************************************************/


OApplication::OApplication( int& argc, char** argv, const QCString& rAppName )
             :QPEApplication( argc, argv ),
             _appname( rAppName ),
             _config( 0 )
{
    init();
}


OApplication::~OApplication()
{
    delete d;
    if ( _config )
        delete _config;
    OApplication::_instance = 0;
    // after deconstruction of the one-and-only application object,
    // the construction of another object is allowed
}


OConfig* OApplication::config()
{
    if ( !_config )
    {
        _config = new OConfig( _appname );
    }
    return _config;
}


void OApplication::init()
{
    d = new OApplicationPrivate();
    if ( !OApplication::_instance )
    {
        OApplication::_instance = this;

        /* register SIGSEGV handler to give programs an option
         * to exit gracefully, e.g. save or close devices or files.
        struct sigaction sa;
        sa.sa_handler = ( void (*)(int) ) &segv_handler;
        sa.sa_flags = SA_SIGINFO | SA_RESTART;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGSEGV, &sa, NULL);
        */
    }
    else
    {
        qFatal( "OApplication: Can't create more than one OApplication object. Aborting." );
    }
}


void OApplication::showMainWidget( QWidget* widget, bool nomax )
{
    QPEApplication::showMainWidget( widget, nomax );
    widget->setCaption( _appname );
}


void OApplication::setTitle( const QString& title ) const
{
    if ( mainWidget() )
    {
        if ( !title.isNull() )
            mainWidget()->setCaption( QString(_appname) + QString( " - " ) + title );
        else
            mainWidget()->setCaption( _appname );
    }
}

