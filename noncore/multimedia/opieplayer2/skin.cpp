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
#include <qtimer.h>

#include <qpe/resource.h>
#include <qpe/config.h>

#include <assert.h>

struct SkinData
{
    typedef QMap<QString, QImage> ButtonMaskImageMap;

    QPixmap backgroundPixmap;
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
    typedef QCache<QPixmap> BackgroundPixmapCache;

    template <class CacheType>
    void store( const QCache<CacheType> &cache, const QString &key, CacheType *data );

    DataCache m_cache;
    BackgroundPixmapCache m_backgroundPixmapCache;
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
    if ( m_isCachable )
        SkinCache::self().store( m_skinPath, m_fileNameInfix, d );
    else
        delete d;
}

void Skin::init( const QString &name )
{
    m_isCachable = true;
    m_skinPath = "opieplayer2/skins/" + name;
    d = SkinCache::self().lookupAndTake( m_skinPath, m_fileNameInfix );
}

QPixmap Skin::backgroundPixmap() const
{
    if ( d->backgroundPixmap.isNull() )
        d->backgroundPixmap = loadImage( QString( "%1/background" ).arg( m_skinPath ) );
    return d->backgroundPixmap;
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
    m_backgroundPixmapCache.setMaxCost( 1 );
}

SkinData *SkinCache::lookupAndTake( const QString &skinPath, const QString &fileNameInfix )
{
    QString key = skinPath + fileNameInfix;

    SkinData *data = m_cache.take( key );
    if ( !data )
        data = new SkinData;
    else
        qDebug( "SkinCache: hit" );

    QPixmap *bgPixmap = m_backgroundPixmapCache.find( skinPath );
    if ( bgPixmap ) {
        qDebug( "SkinCache: hit on bgpixmap" );
        data->backgroundPixmap = *bgPixmap;
    }
    else
        data->backgroundPixmap = QPixmap();

    return data;
}

void SkinCache::store( const QString &skinPath, const QString &fileNameInfix, SkinData *data )
{
    QPixmap *backgroundPixmap = new QPixmap( data->backgroundPixmap );

    data->backgroundPixmap = QPixmap();

    QString key = skinPath + fileNameInfix;

    if ( m_cache.find( key, false /*ref*/ ) != 0 ||
         !m_cache.insert( key, data ) )
        delete data;

    if ( m_backgroundPixmapCache.find( skinPath, false /*ref*/ ) != 0 ||
         !m_backgroundPixmapCache.insert( skinPath, backgroundPixmap ) )
        delete backgroundPixmap;
}

SkinLoader::IncrementalLoader::IncrementalLoader( const Info &info )
    : m_skin( info.skinName, info.fileNameInfix ), m_info( info )
{
    m_currentState = LoadBackgroundPixmap;
}

SkinLoader::IncrementalLoader::LoaderResult SkinLoader::IncrementalLoader::loadStep()
{
    switch ( m_currentState ) {
        case LoadBackgroundPixmap:
            qDebug( "load bgpixmap" );
            m_skin.backgroundPixmap();
            m_currentState = LoadButtonUpImage;
            break;
        case LoadButtonUpImage:
            qDebug( "load upimage" );
            m_skin.buttonUpImage();
            m_currentState = LoadButtonDownImage;
            break;
        case LoadButtonDownImage:
            qDebug( "load downimage" );
            m_skin.buttonDownImage();
            m_currentState = LoadButtonMasks;
            m_currentButton = 0;
            break;
        case LoadButtonMasks:
            qDebug( "load button masks %i", m_currentButton );
            m_skin.buttonMaskImage( m_info.buttonInfo[ m_currentButton ].fileName );

            m_currentButton++;
            if ( m_currentButton >= m_info.buttonCount )
                m_currentState = LoadButtonMask;

            break;
        case LoadButtonMask:
            qDebug( "load whole mask" );
            m_skin.buttonMask( m_info.buttonInfo, m_info.buttonCount );
            return LoadingCompleted;
    }

    return MoreToCome;
}

SkinLoader::SkinLoader()
    : m_currentLoader( 0 ), m_timerId( -1 )
{
}

SkinLoader::~SkinLoader()
{
    qDebug( "SkinLoader::~SkinLoader()" );
    killTimers();
    delete m_currentLoader;
}

void SkinLoader::schedule( const MediaWidget::GUIInfo &guiInfo )
{
    schedule( Skin::defaultSkinName(), guiInfo );
}

void SkinLoader::schedule( const QString &skinName, const MediaWidget::GUIInfo &guiInfo )
{
    pendingSkins << Info( skinName, guiInfo );
}

void SkinLoader::start()
{
    assert( m_timerId == -1 );
    m_timerId = startTimer( 100 /* ms */ );
    qDebug( "SkinLoader::start() %d jobs", pendingSkins.count() );
}

void SkinLoader::timerEvent( QTimerEvent *ev )
{
    if ( ev->timerId() != m_timerId ) {
        QObject::timerEvent( ev );
        return;
    }

    if ( !m_currentLoader ) {

        if ( pendingSkins.isEmpty() ) {
            qDebug( "all jobs done" );
            killTimer( m_timerId );
            m_timerId = -1;
            // ### qt3: use deleteLater();
            QTimer::singleShot( 0, this, SLOT( deleteMe() ) );
            return;
        }

        Info nfo = *pendingSkins.begin();
        pendingSkins.remove( pendingSkins.begin() );

        m_currentLoader = new IncrementalLoader( nfo );
        qDebug( "new loader %i jobs left", pendingSkins.count() );
    }

    if ( m_currentLoader->loadStep() == IncrementalLoader::LoadingCompleted ) {
        delete m_currentLoader;
        m_currentLoader = 0;
    }

    qDebug( "finished step" );
}

void SkinLoader::deleteMe()
{
    delete this;
}

/* vim: et sw=4 ts=4
 */
