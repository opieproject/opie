/*
                             This file is part of the OPIE Project
               =.            Copyright (c)  2002 Trolltech AS <info@trolltech.com>
             .=l.            Copyright (c)  2003 Michael Lauer <mickeyl@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "doctabsettings.h"

#include <qpe/config.h>

#include <qcheckbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>

DocTabSettings::DocTabSettings( QWidget *parent, const char *name ):QWidget( parent, name )
{
    QBoxLayout *lay = new QVBoxLayout( this, 4, 4 );

    _enable = new QCheckBox( tr( "Enable the Documents Tab" ), this );

    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    _enable->setChecked( cfg.readBoolEntry( "Enable", true ) );

    lay->addWidget( _enable );
    lay->addWidget( new QLabel( tr( "<b>Note:</b> Changing these settings may need restarting Opie to become effective." ), this ) );

    lay->addStretch();

    QWhatsThis::add( _enable, tr( "Check, if you want the Documents Tab to be visible." ) );
}

void DocTabSettings::appletChanged()
{
}

void DocTabSettings::accept()
{
    qDebug( "DocTabSettings::accept()" );
    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    cfg.writeEntry( "Enable", _enable->isChecked() );
    cfg.write();
}

