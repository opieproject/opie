
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
