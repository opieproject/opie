/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#define QTOPIA_INTERNAL_MIMEEXT
#include <qpe/qpeapplication.h>
#include "resource.h"
#include "mimetype.h"
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>
#include <qpixmapcache.h>
#include <qpainter.h>

// this namespace is just a workaround for a gcc bug
// gcc exports inline functions in the generated file
// inlinepics_p.h

namespace {
#include "inlinepics_p.h"
}

/*!
  \class Resource resource.h
  \brief The Resource class provides access to named resources.

  The resources may be provided from files or other sources.

  The allSounds() function returns a list of all the sounds available.
  A particular sound can be searched for using findSound().

  Images can be loaded with loadImage(), loadPixmap(), loadBitmap()
  and loadIconSet().

  \ingroup qtopiaemb
*/

/*!
  \fn Resource::Resource()
  \internal
*/

/*!
  Returns the QPixmap called \a pix. You should avoid including
  any filename type extension (e.g. .png, .xpm).
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
  Returns the QBitmap called \a pix. You should avoid including
  any filename type extension (e.g. .png, .xpm).
*/
QBitmap Resource::loadBitmap( const QString &pix )
{
    QBitmap bm;
    bm = loadPixmap(pix);
    return bm;
}

/*!
  Returns the filename of a pixmap called \a pix. You should avoid including
  any filename type extension (e.g. .png, .xpm).

  Normally you will use loadPixmap() rather than this function.
*/
QString Resource::findPixmap( const QString &pix )
{
    QString picsPath = QPEApplication::qpeDir() + "pics/";

    QString f;

    // Common case optimizations...
    f = picsPath + pix + ".png";
    if ( QFile( f ).exists() )
	return f;
    f = picsPath + pix + ".xpm";
    if ( QFile( f ).exists() )
	return f;

    // All formats...
    QStrList fileFormats = QImageIO::inputFormats();
    QString ff = fileFormats.first();
    while ( fileFormats.current() ) {
	QStringList exts = MimeType("image/"+ff.lower()).extensions();
	for ( QStringList::ConstIterator it = exts.begin(); it!=exts.end(); ++it ) {
	    QString f = picsPath + pix + "." + *it;
	    if ( QFile(f).exists() )
		return f;
	}
	ff = fileFormats.next();
    }

    // Finally, no (or existing) extension...
    if ( QFile( picsPath + pix ).exists() )
	return picsPath + pix;

    //qDebug("Cannot find pixmap: %s", pix.latin1());
    return QString();
}

/*!
  Returns a sound file for a sound called \a name.

  You should avoid including any filename type extension (e.g. .wav),
  as the system will search for only those fileformats which are supported
  by the library.

  Currently, only WAV files are supported.
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

static QImage load_image(const QString &name)
{
    QImage img = qembed_findImage(name.latin1());
    if ( img.isNull() ) {
	// No inlined image, try file
	QString f = Resource::findPixmap(name);
	if ( !f.isEmpty() )
	    img.load(f);
    }
    return img;
}

/*!
  Returns the QImage called \a name. You should avoid including
  any filename type extension (e.g. .png, .xpm).
*/
QImage Resource::loadImage( const QString &name)
{
#ifndef QT_NO_DEPTH_32	// have alpha-blended pixmaps
    static QImage last_enabled;
    static QString last_enabled_name;
    if ( name == last_enabled_name )
	return last_enabled;
#endif
    QImage img = load_image(name);
#ifndef QT_NO_DEPTH_32	// have alpha-blended pixmaps
    if ( img.isNull() ) {
	// No file, try generating
	if ( name[name.length()-1]=='d' && name.right(9)=="_disabled" ) {
	    last_enabled_name = name.left(name.length()-9);
	    last_enabled = load_image(last_enabled_name);
	    if ( last_enabled.isNull() ) {
		last_enabled_name = QString::null;
	    } else {
		img.detach();
		img.create( last_enabled.width(), last_enabled.height(), 32 );
		for ( int y = 0; y < img.height(); y++ ) {
		    for ( int x = 0; x < img.width(); x++ ) {
			QRgb p = last_enabled.pixel( x, y );
			int a = qAlpha(p)/3;
			int g = qGray(qRed(p),qGreen(p),qBlue(p));
			img.setPixel( x, y, qRgba(g,g,g,a) );
		    }
		}
		img.setAlphaBuffer( TRUE );
	    }
	}
    }
#endif
    return img;
}

/*!
  \fn QIconSet Resource::loadIconSet( const QString &name )

  Returns a QIconSet for the pixmap named \a name.  A disabled icon is
  generated that conforms to the Qtopia look & feel.  You should avoid
  including any filename type extension (eg. .png, .xpm).
*/
