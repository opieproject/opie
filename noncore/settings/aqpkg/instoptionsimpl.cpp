/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
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

#include "global.h"
#include "instoptionsimpl.h"
#include "ipkg.h"

/* OPIE */
#ifdef QWS
#include <qpe/config.h>
#endif
#include <qpe/qpeapplication.h>

/* QT */
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>

InstallOptionsDlgImpl::InstallOptionsDlgImpl( int flags, int verb, QWidget * parent, const char* name, bool modal, WFlags fl )
        : QDialog( parent, name, modal, fl )
{
    setCaption( tr( "Options" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 2 );
    layout->setSpacing( 4 );

    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "Options" ), this );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addWidget( grpbox );

    QVBoxLayout *grplayout = new QVBoxLayout( grpbox->layout() );

    forceDepends = new QCheckBox( tr( "Force Depends" ), grpbox );
    grplayout->addWidget( forceDepends );

    forceReinstall = new QCheckBox( tr( "Force Reinstall" ), grpbox );
    grplayout->addWidget( forceReinstall );

    forceRemove = new QCheckBox( tr( "Force Remove" ), grpbox );
    grplayout->addWidget( forceRemove );

    forceOverwrite = new QCheckBox( tr( "Force Overwrite" ), grpbox );
    grplayout->addWidget( forceOverwrite );

    QLabel *l = new QLabel( tr( "Information Level" ), grpbox );
    grplayout->addWidget( l );

    verboseIpkg = new QComboBox( grpbox );
    verboseIpkg->insertItem( tr( "Errors only" ) );
    verboseIpkg->insertItem( tr( "Normal messages" ) );
    verboseIpkg->insertItem( tr( "Informative messages" ) );
    verboseIpkg->insertItem( tr( "Troubleshooting output" ) );
    verboseIpkg->setCurrentItem( verb );
    grplayout->addWidget( verboseIpkg );

    grplayout->addItem( new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    if ( flags & FORCE_DEPENDS )
        forceDepends->setChecked( true );
    if ( flags & FORCE_REINSTALL )
        forceReinstall->setChecked( true );
    if ( flags & FORCE_REMOVE )
        forceRemove->setChecked( true );
    if ( flags & FORCE_OVERWRITE )
        forceOverwrite->setChecked( true );
    //    if ( flags & VERBOSE_WGET )
    //    	verboseWget->setChecked( true );
    //    if ( flags & MAKE_LINKS )
    //    	makeLinks->setChecked( true );

    QPEApplication::showDialog( this );

}

InstallOptionsDlgImpl::~InstallOptionsDlgImpl()
{}


int InstallOptionsDlgImpl :: getFlags()
{
    int flags = 0;

    if ( forceDepends->isChecked() )
        flags |= FORCE_DEPENDS;
    if ( forceReinstall->isChecked() )
        flags |= FORCE_REINSTALL;
    if ( forceRemove->isChecked() )
        flags |= FORCE_REMOVE;
    if ( forceOverwrite->isChecked() )
        flags |= FORCE_OVERWRITE;
    if ( verboseWget->isChecked() )
        flags |= VERBOSE_WGET;

    return flags;
}

int InstallOptionsDlgImpl :: getInfoLevel()
{
    return verboseIpkg->currentItem();
}
