/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 OPIE team <opie@handhelds.org?>
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

#include "packagewin.h"
#include "package.h"
#include "datamgr.h"

#include <qpe/resource.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qtextview.h>

PackageWindow::PackageWindow( Package *package, const QString &server )
	: QWidget( 0, 0, 0 )
{
	QString str;
	if ( package )
	{
		setCaption( package->getPackageName() );
		QString destName;
		if ( package->getLocalPackage() )
		{
			if ( package->getLocalPackage()->getInstalledTo() )
				destName = package->getLocalPackage()->getInstalledTo()->getDestinationName();
		}
		else
		{
			if ( package->getInstalledTo() )
				destName = package->getInstalledTo()->getDestinationName();
		}

		if ( !package->isPackageStoredLocally() )
		{
			str.append( tr( "<b>Description</b> - " ) );
			str.append( package->getDescription() );
			if ( !destName.isNull() )
			{
				str.append( tr( "<p><b>Installed To</b> - " ) );
				str.append( destName );
			}
			str.append( tr( "<p><b>Size</b> - " ) );
			str.append( package->getPackageSize() );
			str.append( tr( "<p><b>Section</b> - " ) );
			str.append( package->getSection() );
		}
		else
		{
			str.append( tr( "<p><b>Filename</b> - " ) );
			str.append( package->getFilename() );
		}

		if ( server == LOCAL_SERVER )
		{
			str.append( tr( "<p><b>Version Installed</b> - " ) );
			str.append( package->getVersion() );
		}
		else
		{
			str.append( tr( "<p><b>Version Available</b> - " ) );
			str.append( package->getVersion() );
			if ( package->getLocalPackage() )
			{
				if ( package->isInstalled() )
				{
					str.append( tr( "<p><b>Version Installed</b> - " ) );
					str.append( package->getInstalledVersion() );
				}
			}
		}
	}
	else
	{
		setCaption( tr( "Package Information" ) );
		str = tr( "Package information is unavailable" );
	}

	QVBoxLayout *layout = new QVBoxLayout( this, 4, 4 );

	QTextView *l = new QTextView( str, QString::null, this );
	//l->setTextFormat( Qt::RichText );
	//l->setAlignment( Qt::AlignLeft | Qt::AlignVCenter | Qt::WordBreak );
	layout->addWidget( l );

	QPushButton *btn = new QPushButton( Resource::loadPixmap( "enter" ), tr( "Close" ), this );
	layout->addWidget( btn );
	connect( btn, SIGNAL( clicked() ), this, SLOT( close() ) );

}

PackageWindow::~PackageWindow()
{
}
