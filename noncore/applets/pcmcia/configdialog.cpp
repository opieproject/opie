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
#include <opie2/opcmciasystem.h>
using namespace Opie::Core;

/* QT */
#include <qcombobox.h>
#include <qdir.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qtextstream.h>

ConfigDialog::ConfigDialog( const OPcmciaSocket* card, QWidget* parent )
             :ConfigDialogBase( parent, "pcmcia config dialog", true )
{
    gbDetails->setTitle( QString( "Details for card in socket #%1" ).arg( card->number() ) );
    txtCardName->setText( card->productIdentity().join( " " ) );
    txtManfid->setText( card->manufacturerIdentity() );
    txtFunction->setText( card->function() );

    QString insertAction = preferredAction( card, "insert" );
    QString resumeAction = preferredAction( card, "resume" );

    odebug << "pcmcia: preferred insertion action for card '" << card->name() << "' seems to be '" << insertAction << "'" << oendl;
    odebug << "pcmcia: preferred resume    action for card '" << card->name() << "' seems to be '" << resumeAction << "'" << oendl;

    if ( !insertAction.isEmpty() )
    {
        for ( int i; i < cbInsertAction->count(); ++i )
            if ( cbInsertAction->text( i ) == insertAction ) cbInsertAction->setCurrentItem( i );
    }

    if ( !resumeAction.isEmpty() )
    {
        for ( int i; i < cbResumeAction->count(); ++i )
            if ( cbResumeAction->text( i ) == resumeAction ) cbResumeAction->setCurrentItem( i );
    }

    if ( !card->isUnsupported() )
    {
        odebug << "pcmcia: card is recognized - hiding bindings" << oendl;
        textInfo->hide();
        textBindTo->hide();
        cbBindTo->hide();
        return;
    }
    else
    {
        odebug << "card is unsupported yet - showing possible bindings" << oendl;
        textInfo->show();
        textBindTo->show();
        cbBindTo->show();
    }

    // parse possible bind entries out of /etc/pcmcia/*.conf
    QDir pcmciaconfdir( "/etc/pcmcia", "*.conf" );

    for ( unsigned int i = 0; i < pcmciaconfdir.count(); ++i )
    {
        odebug << "processing conf file '" << pcmciaconfdir[i] << "'" << oendl;
        QString conffilename = QString( "%1/%2" ).arg( pcmciaconfdir.absPath() ).arg( pcmciaconfdir[i] );
        QFile conffile( conffilename );
        if ( conffile.open( IO_ReadOnly ) )
        {
            QTextStream ts( &conffile );
            while ( !ts.atEnd() )
            {
                QString word;
                ts >> word;
                if ( word == "bind" )
                {
                    word = ts.readLine();
                    bindEntries[ word.stripWhiteSpace() ] = conffilename;
                    continue;
                }
                ts.readLine();
            }
        }
        else
        {
            owarn << "couldn't open '" << conffile.name() << "' for reading" << oendl;
            continue;
        }
    }

    for ( StringMap::Iterator it = bindEntries.begin(); it != bindEntries.end(); ++it )
    {
        odebug << "found binding '" << it.key() << "' defined in '" << it.data().latin1() << "'" << oendl;
        cbBindTo->insertItem( it.key() );
    }
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::writeConfigEntry( const OPcmciaSocket* card, const QString& key, const QString& value )
{
    OConfig cfg( "PCMCIA" );
    cfg.setGroup( "Global" );
    int nCards = cfg.readNumEntry( "nCards", 0 );
    QString cardName = card->productIdentity().join( " " ).stripWhiteSpace();
    QString action;

    for ( int i = 0; i < nCards; ++i )
    {
        QString cardSection = QString( "Card_%1" ).arg( i );
        cfg.setGroup( cardSection );
        QString name = cfg.readEntry( "name" );
        odebug << "comparing card '" << cardName << "' with known card '" << name << "'" << oendl;
        if ( cardName == name )
        {
            cfg.writeEntry( key, value );
            break;
        }
    }
}

QString ConfigDialog::readConfigEntry( const OPcmciaSocket* card, const QString& key, const QString& defaultValue )
{
    OConfig cfg( "PCMCIA" );
    cfg.setGroup( "Global" );
    int nCards = cfg.readNumEntry( "nCards", 0 );
    QString cardName = card->productIdentity().join( " " ).stripWhiteSpace();
    QString value;

    for ( int i = 0; i < nCards; ++i )
    {
        QString cardSection = QString( "Card_%1" ).arg( i );
        cfg.setGroup( cardSection );
        QString name = cfg.readEntry( "name" );
        odebug << "comparing card '" << cardName << "' with known card '" << name << "'" << oendl;
        if ( cardName == name )
        {
            value = cfg.readEntry( key, defaultValue );
            break;
        }
    }
    return value;
}


QString ConfigDialog::preferredAction( const OPcmciaSocket* card, const QString& type )
{
    return ConfigDialog::readConfigEntry( card, QString( "%1Action" ).arg( type ), "suspend" );
}
