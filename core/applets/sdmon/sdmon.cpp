/**********************************************************************
** Copyright (C) 2001 Devin Butterfield.  All rights reserved.

** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

/*
 * Adapted from cardmon to monitor sd/mm cards on the Zaurus
 * by Charles-Edouard Ruault <ce@ruault.com> 02/01/2002
 */


#include "sdmon.h"

#include <qpe/resource.h>

#include <qcopchannel_qws.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

SdMonitor::SdMonitor( QWidget *parent ) : QWidget( parent ),
	img( Resource::loadImage( "sdmon/sdcard.png" ) ), cardIn( FALSE )
{
    QCopChannel* sdChannel = new QCopChannel( "QPE/Card", this );
    connect( sdChannel, SIGNAL(received(const QCString &, const QByteArray &)),
             this, SLOT(cardMessage( const QCString &, const QByteArray &)) );

    setFixedSize( img.size() );
    hide();
    getStatus();
}

SdMonitor::~SdMonitor()
{
}

void SdMonitor::mousePressEvent( QMouseEvent * )
{
    QPopupMenu *menu = new QPopupMenu();
    QString cmd;
    int err=0;
    menu->insertItem( tr("Eject card"), 0 );
    QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
    
    if ( menu->exec( p, 1 ) == 0 ) 
	{
	  cmd = "/etc/sdcontrol compeject";
	  err = system( (const char *) cmd );
	  if ( ( err != 0 ) ) 
	  {
		qDebug("Could not execute `/etc/sdcontrol compeject'! err=%d", err); 
		QMessageBox::warning( this, tr("SdMonitor"), tr("Card eject failed!"),
								tr("&OK") );
	  }
    }
    delete menu;
}


void SdMonitor::cardMessage( const QCString &msg, const QByteArray & )
{

    if ( msg == "mtabChanged()" ) 
	{
	  if ( getStatus() ) repaint(FALSE);
    }
}


bool SdMonitor::getStatus( void )
{
  bool cardWas=cardIn; // remember last state
  cardIn=false;
  
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );
	
    if ( mntfp ) 
	{
	  while ( (me = getmntent( mntfp )) != 0 ) 
	  {
	    QString fs = me->mnt_fsname;
	    if ( fs.left(7)=="/dev/sd" || fs.left(9) == "/dev/mmcd" )
		  cardIn=true;
	  }
	  endmntent( mntfp );
	  if (cardIn!=cardWas)
	  {
		if (cardIn)
		  show();
		else
		  hide();
	  }
	}
	else
	{
	  hide();
	}
#else 
#error "Not on Linux"
#endif
    return ((cardWas == cardIn) ? FALSE : TRUE);
}

void SdMonitor::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    
    if ( cardIn ) 
	{
	  p.drawImage( 0, 0, img );
    }
    else 
	{
	  p.eraseRect( rect() );
    }
}


