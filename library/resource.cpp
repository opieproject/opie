/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qpeapplication.h"
#include "resource.h"
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>
#include <qpixmapcache.h>
#include <qpainter.h>

#include "inlinepics_p.h"

/*!
  \class Resource resource.h
  \brief The Resource class provides access to named resources.

  The resources may be provided from files or other sources.
*/

/*!
  \fn Resource::Resource()
  \internal
*/

/*!
  Returns the QPixmap named \a pix. You should avoid including
  any filename type extension (eg. .png, .xpm).
*/
QPixmap Resource::loadPixmap( const QString &pix ) 
{
    QPixmap pm;
    QString key="QPE_"+pix;
    if ( !QPixmapCache::find(key,pm) ) {
	pm.convertFromImage(loadImage(pix));
	QPixmapCache::insert(key,pm);
    }
    return pm;
}

/*!
  Returns the QBitmap named \a pix. You should avoid including
  any filename type extension (eg. .png, .xpm).
*/
QBitmap Resource::loadBitmap( const QString &pix ) 
{
    QBitmap bm;
    bm = loadPixmap(pix);
    return bm;
}

/*!
  Returns the filename of a pixmap named \a pix. You should avoid including
  any filename type extension (eg. .png, .xpm .jpg .jpeg).

  Normally you will use loadPixmap() rather than this function.
*/
QString Resource::findPixmap( const QString &pix )
{
    QString picsPath = QPEApplication::qpeDir() + "pics/";

    if ( QFile( picsPath + pix + ".png").exists() )
	return picsPath + pix + ".png";
    else if ( QFile( picsPath + pix + ".jpeg").exists() )
	return picsPath + pix + ".jpeg";
    else if ( QFile( picsPath + pix + ".jpg").exists() )
	return picsPath + pix + ".jpg";
    else if ( QFile( picsPath + pix + ".xpm").exists() )
	return picsPath + pix + ".xpm";
    else if ( QFile( picsPath + pix ).exists() )
	return picsPath + pix;

    //qDebug("Cannot find pixmap: %s", pix.latin1());
    return QString();
}

/*!
  Returns a sound file for a sound named \a name.
  You should avoid including any filename type extension (eg. .wav, .au, .mp3).
*/
QString Resource::findSound( const QString &name )
{
    QString picsPath = QPEApplication::qpeDir() + "sounds/";

    QString result;
    if ( QFile( (result = picsPath + name + ".wav") ).exists() )
	return result;

    return QString();
}

/*!
  Returns a list of all sound names.
*/
QStringList Resource::allSounds()
{
    QDir resourcedir( QPEApplication::qpeDir() + "sounds/", "*.wav" );
    QStringList entries = resourcedir.entryList();
    QStringList result;
    for (QStringList::Iterator i=entries.begin(); i != entries.end(); ++i)
	result.append((*i).replace(QRegExp("\\.wav"),""));
    return result;
}

/*!
  Returns the QImage named \a name. You should avoid including
  any filename type extension (eg. .png, .xpm .jpg).
*/
QImage Resource::loadImage( const QString &name)
{
    QImage img = qembed_findImage(name.latin1());
    if ( img.isNull() )
	return QImage(findPixmap(name));
    return img;
}

/*!
  \fn QIconSet Resource::loadIconSet( const QString &name ) 

  Returns a QIconSet for the pixmap named \a name.  A disabled icon is
  generated that conforms to the Qtopia look & feel.  You should avoid
  including any filename type extension (eg. .png, .xpm).
*/
