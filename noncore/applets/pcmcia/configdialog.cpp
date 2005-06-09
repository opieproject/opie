/*
                             This file is part of the Opie Project
             =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "configdialog.h"

/* OPIE */
#include <opie2/oconfig.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qcombobox.h>
#include <qdir.h>
#include <qlabel.h>

ConfigDialog::ConfigDialog( const QString& cardname, QWidget* parent )
             :ConfigDialogBase( parent, "pcmcia config dialog", true )
{
    //setCaption( tr( "Configure %1" ).arg( cardname ) );
    textCardName->setText( cardname );

    OConfig cfg( "PCMCIA" );
    cfg.setGroup( "Global" );
    int nCards = cfg.readNumEntry( "nCards", 0 );
    QString insert;

    for ( int i = 0; i < nCards; ++i )
    {
        QString cardSection = QString( "Card_%1" ).arg( i );
        cfg.setGroup( cardSection );
        QString name = cfg.readEntry( "name" );
        odebug << "comparing card '" << cardname << "' with known card '" << name << "'" << oendl;
        if ( cardname == name )
        {
            insert = cfg.readEntry( "insert" );
            break;
        }
    }
    odebug << "preferred action for card '" << cardname << "' seems to be '" << insert << "'" << oendl;

    if ( !insert.isEmpty() )
    {
        for ( int i; i < cbAction->count(); ++i )
            if ( cbAction->text( i ) == insert ) cbAction->setCurrentItem( i );
    }

    // parse possible device and class names out of /etc/pcmcia/*.conf
    QStringList deviceNames;
    QStringList classNames;

    QDir pcmciaconfdir( "/etc/pcmcia", "*.conf" );

    for ( int i = 0; i < pcmciaconfdir.count(); ++i )
        odebug << "found conf file '" << pcmciaconfdir[i] << "'" << oendl;

}

ConfigDialog::~ConfigDialog()
{
}
