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

#include <qpe/resource.h>
#include <qpe/config.h>

#include <assert.h>

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

void Skin::init( const QString &name )
{
    m_skinPath = "opieplayer2/skins/" + name;
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
    if ( m_backgroundImage.isNull() )
        m_backgroundImage = SkinCache::self().loadImage( QString( "%1/background" ).arg( m_skinPath ) );
    return m_backgroundImage;
}

QImage Skin::buttonUpImage() const
{
    if ( m_buttonUpImage.isNull() )
        m_buttonUpImage = SkinCache::self().loadImage( QString( "%1/skin%2_up" ).arg( m_skinPath ).arg( m_fileNameInfix ) );
    return m_buttonUpImage;
}

QImage Skin::buttonDownImage() const
{
    if ( m_buttonDownImage.isNull() )
        m_buttonDownImage = SkinCache::self().loadImage( QString( "%1/skin%2_down" ).arg( m_skinPath ).arg( m_fileNameInfix ) );
    return m_buttonDownImage;
}

QImage Skin::buttonMask( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount ) const
{
    if ( !m_buttonMask.isNull() )
        return m_buttonMask;

    QSize buttonAreaSize = buttonUpImage().size();

    m_buttonMask = QImage( buttonAreaSize, 8, 255 );
    m_buttonMask.fill( 0 );

    for ( uint i = 0; i < buttonCount; ++i )
        addButtonToMask( skinButtonInfo[ i ].command + 1, buttonMaskImage( skinButtonInfo[ i ].fileName ) );

    return m_buttonMask;
}

void Skin::addButtonToMask( int tag, const QImage &maskImage ) const
{
    if ( maskImage.isNull() )
        return;

    uchar **dest = m_buttonMask.jumpTable();
    for ( int y = 0; y < m_buttonMask.height(); y++ ) {
        uchar *line = dest[y];
        for ( int x = 0; x < m_buttonMask.width(); x++ )
            if ( !qRed( maskImage.pixel( x, y ) ) )
                line[x] = tag;
    }
}

QImage Skin::buttonMaskImage( const QString &fileName ) const
{
    ButtonMaskImageMap::Iterator it = m_buttonMasks.find( fileName );
    if ( it == m_buttonMasks.end() ) {
        QString prefix = m_skinPath + QString::fromLatin1( "/skin%1_mask_" ).arg( m_fileNameInfix );
        QString path = prefix + fileName + ".png";
        it = m_buttonMasks.insert( fileName, SkinCache::self().loadImage( path ) );
    }
    return *it;
}

QString Skin::defaultSkinName()
{
    Config cfg( "OpiePlayer" );
    cfg.setGroup( "Options" );
    return cfg.readEntry( "Skin", "default" );
}

SkinCache::SkinCache()
{
    m_cache.setAutoDelete( true );
}

QImage SkinCache::loadImage( const QString &name )
{
    ThreadUtil::AutoLock lock( m_cacheGuard );

    QImage *image = m_cache.find( name );
    if ( image ) {
        qDebug( "cache hit for %s", name.ascii() );
        return *image;
    }

    image = new QImage( Resource::findPixmap( name ) );
    m_cache.insert( name, image );
    return *image;
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
