/*
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>
             (C) 2002 Max Reiss <harlekin@handhelds.org>
             (C) 2002 L. Potter <ljp@llornkcor.com>
             (C) 2002 Holger Freyther <zecke@handhelds.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "mediawidget.h"
#include "playlistwidget.h"
#include "skin.h"

#include <qpe/config.h>
#include <qpe/qpeapplication.h>

MediaWidget::MediaWidget( PlayListWidget &_playList, MediaPlayerState &_mediaPlayerState, QWidget *parent, const char *name )
    : QWidget( parent, name ), mediaPlayerState( _mediaPlayerState ), playList( _playList )
{
    connect( &mediaPlayerState, SIGNAL( displayTypeChanged( MediaPlayerState::DisplayType ) ),
             this, SLOT( setDisplayType( MediaPlayerState::DisplayType ) ) );
    connect( &mediaPlayerState, SIGNAL( lengthChanged( long ) ),
             this, SLOT( setLength( long ) ) );
    connect( &mediaPlayerState, SIGNAL( playingToggled( bool ) ),
             this, SLOT( setPlaying( bool ) ) );
}

MediaWidget::~MediaWidget()
{
}

void MediaWidget::setupButtons( const SkinButtonInfo *skinInfo, uint buttonCount,
                                const Skin &skin )
{
    buttonMask = skin.buttonMask( skinInfo, buttonCount );

    buttons.clear();
    buttons.reserve( buttonCount );

    for ( uint i = 0; i < buttonCount; ++i ) {
        Button button = setupButton( skinInfo[ i ], skin );
        buttons.push_back( button );
    }
}

MediaWidget::Button MediaWidget::setupButton( const SkinButtonInfo &buttonInfo, const Skin &skin )
{
    Button button;
    button.command = buttonInfo.command;
    button.type = buttonInfo.type;
    button.mask = skin.buttonMaskImage( buttonInfo.fileName );

    return button;
}

void MediaWidget::loadDefaultSkin( const SkinButtonInfo *skinInfo, uint buttonCount, const QString &fileNameInfix )
{
    Config cfg( "OpiePlayer" );
    cfg.setGroup( "Options" );
    QString skin = cfg.readEntry( "Skin","default" );

    loadSkin( skinInfo, buttonCount, skin, fileNameInfix );
}

void MediaWidget::loadSkin( const SkinButtonInfo *skinInfo, uint buttonCount, const QString &name, const QString &fileNameInfix )
{
    Skin skin( name, fileNameInfix );

    QString skinPath = "opieplayer2/skins/" + name;
    backgroundPixmap = skin.backgroundImage();
    buttonUpImage = skin.buttonUpImage();
    buttonDownImage = skin.buttonDownImage();

    setupButtons( skinInfo, buttonCount, skin );
}

void MediaWidget::closeEvent( QCloseEvent * )
{
    mediaPlayerState.setList();
}

void MediaWidget::paintEvent( QPaintEvent *pe )
{
    QPainter p( this );

    if ( mediaPlayerState.isFullscreen() ) {
        // Clear the background
        p.setBrush( QBrush( Qt::black ) );
        return;
    } 

    if ( !pe->erased() ) {
        // Combine with background and double buffer
        QPixmap pix( pe->rect().size() );
        QPainter p( &pix );
        p.translate( -pe->rect().topLeft().x(), -pe->rect().topLeft().y() );
        p.drawTiledPixmap( pe->rect(), backgroundPixmap, pe->rect().topLeft() );
        paintAllButtons( p );
        QPainter p2( this );
        p2.drawPixmap( pe->rect().topLeft(), pix );
    } else {
        QPainter p( this );
        paintAllButtons( p );
    }
}

MediaWidget::Button *MediaWidget::buttonAt( const QPoint &position )
{
    if ( position.x() <= 0 || position.y() <= 0 ||
         position.x() >= buttonMask.width() ||
         position.y() >= buttonMask.height() )
        return 0;

    int pixelIdx = buttonMask.pixelIndex( position.x(), position.y() );
    for ( ButtonVector::iterator it = buttons.begin(); it != buttons.end(); ++it )
        if ( it->command + 1 == pixelIdx )
            return &( *it );

    return 0;
}

void MediaWidget::mousePressEvent( QMouseEvent *event )
{
    Button *button = buttonAt( event->pos() - upperLeftOfButtonMask );

    if ( !button ) {
        QWidget::mousePressEvent( event );
        return;
    }

    switch ( button->command ) {
        case VolumeUp:   emit moreClicked(); return;
        case VolumeDown: emit lessClicked(); return;
        case Back:       emit backClicked(); return;
        case Forward:    emit forwardClicked(); return;
        default: break;
    }
}

void MediaWidget::mouseReleaseEvent( QMouseEvent *event )
{
    Button *button = buttonAt( event->pos() - upperLeftOfButtonMask );

    if ( !button ) {
        QWidget::mouseReleaseEvent( event );
        return;
    }

    if ( button->type == ToggleButton )
        toggleButton( *button );

    handleCommand( button->command, button->isDown );
}

void MediaWidget::makeVisible()
{
}

void MediaWidget::handleCommand( Command command, bool buttonDown )
{
    switch ( command ) {
        case Play:       mediaPlayerState.togglePaused(); return;
        case Stop:       mediaPlayerState.setPlaying(FALSE); return;
        case Next:       if( playList.currentTab() == PlayListWidget::CurrentPlayList ) mediaPlayerState.setNext(); return;
        case Previous:   if( playList.currentTab() == PlayListWidget::CurrentPlayList ) mediaPlayerState.setPrev(); return;
        case Loop:       mediaPlayerState.setLooping( buttonDown ); return;
        case VolumeUp:   emit moreReleased(); return;
        case VolumeDown: emit lessReleased(); return;
        case PlayList:   mediaPlayerState.setList();  return;
        case Forward:    emit forwardReleased(); return;
        case Back:       emit backReleased(); return;
        case FullScreen: mediaPlayerState.setFullscreen( true ); makeVisible(); return;
        default: assert( false );
    }
}

bool MediaWidget::isOverButton( const QPoint &position, int buttonId ) const
{
    return ( position.x() > 0 && position.y() > 0 && 
             position.x() < buttonMask.width() && 
             position.y() < buttonMask.height() && 
             buttonMask.pixelIndex( position.x(), position.y() ) == buttonId + 1 );
}

void MediaWidget::paintAllButtons( QPainter &p )
{
    for ( ButtonVector::const_iterator it = buttons.begin();
          it != buttons.end(); ++it )
        paintButton( p, *it );
}

void MediaWidget::paintButton( const Button &button )
{
    QPainter p( this );
    paintButton( p, button );
}

void MediaWidget::paintButton( QPainter &p, const Button &button )
{
    if ( button.isDown )
        p.drawPixmap( upperLeftOfButtonMask, button.pixDown );
    else
        p.drawPixmap( upperLeftOfButtonMask, button.pixUp );
}

void MediaWidget::setToggleButton( Command command, bool down )
{
    for ( ButtonVector::iterator it = buttons.begin(); it != buttons.end(); ++it )
        if ( it->command == command ) {
            setToggleButton( *it, down );
            return;
        }
}

void MediaWidget::setToggleButton( Button &button, bool down )
{
    if ( down != button.isDown )
        toggleButton( button );
}

void MediaWidget::toggleButton( Button &button )
{
    button.isDown = !button.isDown;

    paintButton( button );
}

/* vim: et sw=4 ts=4
 */
