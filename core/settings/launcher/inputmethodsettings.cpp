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

#include "inputmethodsettings.h"

/* OPIE */
#include <qpe/config.h>
#include <opie2/odebug.h>

/* QT */
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>


InputMethodSettings::InputMethodSettings( QWidget *parent, const char *name ):QWidget( parent, name )
{
    QBoxLayout *lay = new QVBoxLayout( this, 4, 4 );

    _resize = new QCheckBox( tr( "Resize application on Popup" ), this );
    _float = new QCheckBox( tr( "Enable floating and resizing" ), this );

    QHBoxLayout* hbox = new QHBoxLayout( lay, 4 );
    hbox->addWidget( new QLabel( "Initial Width:", this ) );
    _size = new QSpinBox( 10, 100, 10, this );
    _size->setSuffix( "%" );
    hbox->addWidget( _size );
    hbox->addStretch();

    Config cfg( "Launcher" );
    cfg.setGroup( "InputMethods" );
    _resize->setChecked( cfg.readBoolEntry( "Resize", true ) );
    _float->setChecked( cfg.readBoolEntry( "Float", false ) );
    _size->setValue( cfg.readNumEntry( "Width", 100 ) );

    lay->addWidget( _resize );
    lay->addWidget( _float );
    lay->addWidget( new QLabel( tr( "<b>Note:</b> Changing these settings may need restarting Opie to become effective." ), this ) );

    lay->addStretch();

    QWhatsThis::add( _resize, tr( "Check, if you want the application to be automatically resized if the input method pops up." ) );
    QWhatsThis::add( _float, tr( "Check, if you want to move and/or resize input methods" ) );
    QWhatsThis::add( _size, tr( "Specify the percentage of the screen width for the input method" ) );
}

void InputMethodSettings::appletChanged()
{
}

void InputMethodSettings::accept()
{
    odebug << "InputMethodSettings::accept()" << oendl;
    Config cfg( "Launcher" );
    cfg.setGroup( "InputMethods" );
    cfg.writeEntry( "Resize", _resize->isChecked() );
    cfg.writeEntry( "Float", _float->isChecked() );
    cfg.writeEntry( "Width", _size->value() );
    cfg.write();
}

