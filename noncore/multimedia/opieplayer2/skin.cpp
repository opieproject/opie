/*
   Copyright (C) 2002 Simon Hausmann <simon@lst.de>
             (C) 2002 Max Reiss <harlekin@handhelds.org>
             (C) 2002 L. Potter <ljp@llornkcor.com>
             (C) 2002 Holger Freyther <zecke@handhelds.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "skin.h"
#include "singleton.h"

#include <qcache.h>
#include <qmap.h>

#include <qpe/resource.h>
#include <qpe/config.h>

#include <assert.h>

struct SkinData
{
    typedef QMap<QString, QImage> ButtonMaskImageMap;

    QImage backgroundImage;
    QImage buttonUpImage;
    QImage buttonDownImage;
    QImage buttonMask;
    ButtonMaskImageMap buttonMasks;
};

class SkinCache : public Singleton<SkinCache>
{
public:
    SkinCache();

    SkinData *lookupAndTake( const QString &skinPath, const QString &fileNameInfix );

    void store( const QString &skinPath, const QString &fileNameInfix, SkinData *data );

private:
    typedef QCache<SkinData> DataCache;
    typedef QCache<QImage> BackgroundImageCache;

    template <class CacheType>
    void store( const QCache<CacheType> &cache, const QString &key, CacheType *data );

    DataCache m_cache;
    BackgroundImageCache m_backgroundImageCache;
};

Skin::Skin( const QString &name, const QString &fileNameInfix )
    : m_fileNameInfix( fileNameInfix )
{
    init( name );
}

Skin::Skin( const QString &fileNameInfix )
    : m_fileNameInfix( fileNameInfix )
{
    init( defaultSkinName() );
}

Skin::~Skin()
{
    SkinCache::self().store( m_skinPath, m_fileNameInfix, d );
}

void Skin::init( const QString &name )
{
    m_skinPath = "opieplayer2/skins/" + name;
    d = SkinCache::self().lookupAndTake( m_skinPath, m_fileNameInfix );
}

void Skin::preload( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount )
{
    backgroundImage();
    buttonUpImage();
    buttonDownImage();
    ( void )buttonMask( skinButtonInfo, buttonCount );
}

QImage Skin::backgroundImage() const
{
    if ( d->backgroundImage.isNull() )
        d->backgroundImage = loadImage( QString( "%1/background" ).arg( m_skinPath ) );
    return d->backgroundImage;
}

QImage Skin::buttonUpImage() const
{
    if ( d->buttonUpImage.isNull() )
        d->buttonUpImage = loadImage( QString( "%1/skin%2_up" ).arg( m_skinPath ).arg( m_fileNameInfix ) );
    return d->buttonUpImage;
}

QImage Skin::buttonDownImage() const
{
    if ( d->buttonDownImage.isNull() )
        d->buttonDownImage = loadImage( QString( "%1/skin%2_down" ).arg( m_skinPath ).arg( m_fileNameInfix ) );
    return d->buttonDownImage;
}

QImage Skin::buttonMask( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount ) const
{
    if ( !d->buttonMask.isNull() )
        return d->buttonMask;

    QSize buttonAreaSize = buttonUpImage().size();

    d->buttonMask = QImage( buttonAreaSize, 8, 255 );
    d->buttonMask.fill( 0 );

    for ( uint i = 0; i < buttonCount; ++i )
        addButtonToMask( skinButtonInfo[ i ].command + 1, buttonMaskImage( skinButtonInfo[ i ].fileName ) );

    return d->buttonMask;
}

void Skin::addButtonToMask( int tag, const QImage &maskImage ) const
{
    if ( maskImage.isNull() )
        return;

    uchar **dest = d->buttonMask.jumpTable();
    for ( int y = 0; y < d->buttonMask.height(); y++ ) {
        uchar *line = dest[y];
        for ( int x = 0; x < d->buttonMask.width(); x++ )
            if ( !qRed( maskImage.pixel( x, y ) ) )
                line[x] = tag;
    }
}

QImage Skin::buttonMaskImage( const QString &fileName ) const
{
    SkinData::ButtonMaskImageMap::Iterator it = d->buttonMasks.find( fileName );
    if ( it == d->buttonMasks.end() ) {
        QString prefix = m_skinPath + QString::fromLatin1( "/skin%1_mask_" ).arg( m_fileNameInfix );
        QString path = prefix + fileName + ".png";
        it = d->buttonMasks.insert( fileName, loadImage( path ) );
    }
    return *it;
}

QString Skin::defaultSkinName()
{
    Config cfg( "OpiePlayer" );
    cfg.setGroup( "Options" );
    return cfg.readEntry( "Skin", "default" );
}

QImage Skin::loadImage( const QString &fileName )
{
    return QImage( Resource::findPixmap( fileName ) );
}

SkinCache::SkinCache()
{
    // let's say we cache two skins (audio+video) at maximum
    m_cache.setMaxCost( 2 );
    // ... and one background pixmap
    m_backgroundImageCache.setMaxCost( 1 );
}

SkinData *SkinCache::lookupAndTake( const QString &skinPath, const QString &fileNameInfix )
{
    QString key = skinPath + fileNameInfix;

    SkinData *data = m_cache.take( key );
    if ( !data )
        data = new SkinData;

    QImage *bgImage = m_backgroundImageCache.find( skinPath );
    if ( bgImage )
        data->backgroundImage = *bgImage;
    else
        data->backgroundImage = QImage();

    return data;
}

void SkinCache::store( const QString &skinPath, const QString &fileNameInfix, SkinData *data )
{
    QImage *backgroundImage = new QImage( data->backgroundImage );

    data->backgroundImage = QImage();

    QString key = skinPath + fileNameInfix;

    if ( m_cache.find( key, false /*ref*/ ) != 0 ||
         !m_cache.insert( key, data ) )
        delete data;

    if ( m_backgroundImageCache.find( skinPath, false /*ref*/ ) != 0 ||
         !m_backgroundImageCache.insert( skinPath, backgroundImage ) )
        delete backgroundImage;
}

SkinLoader::SkinLoader()
{
}

void SkinLoader::schedule( const QString &skinName, const QString &fileNameInfix, 
                           const MediaWidget::SkinButtonInfo *skinButtonInfo, const uint buttonCount )
{
    assert( isRunning() == false );

    pendingSkins << Info( skinName, fileNameInfix, skinButtonInfo, buttonCount );
}

void SkinLoader::run()
{
    qDebug( "SkinLoader::run()" );
    for ( InfoList::ConstIterator it = pendingSkins.begin(); it != pendingSkins.end(); ++it )
        load( *it );
    qDebug( "SkinLoader is done." );
}

void SkinLoader::load( const Info &nfo )
{
    qDebug( "preloading %s with infix %s", nfo.skinName.ascii(), nfo.fileNameInfix.ascii() );

    Skin skin( nfo.skinName, nfo.fileNameInfix );
    skin.preload( nfo.skinButtonInfo, nfo.buttonCount );
}

/* vim: et sw=4 ts=4
 */
