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
#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/config.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <qdir.h>
#include "mediaplayerplugininterface.h"
#include "mediaplayerstate.h"


#ifdef QT_NO_COMPONENT
// Plugins which are compiled in when no plugin architecture available
#include "libmad/libmadpluginimpl.h"
#include "libmpeg3/libmpeg3pluginimpl.h"
#include "wavplugin/wavpluginimpl.h"
#endif


//#define MediaPlayerDebug(x) qDebug x
#define MediaPlayerDebug(x)


MediaPlayerState::MediaPlayerState( QObject *parent, const char *name )
    : QObject( parent, name ), decoder( NULL ), libmpeg3decoder( NULL ) {
    Config cfg( "MediaPlayer" );
    readConfig( cfg );
    loadPlugins();
}


MediaPlayerState::~MediaPlayerState() {
    Config cfg( "MediaPlayer" );
    writeConfig( cfg );
}


void MediaPlayerState::readConfig( Config& cfg ) {
    cfg.setGroup("Options");
    isFullscreen = cfg.readBoolEntry( "FullScreen" );
    isScaled = cfg.readBoolEntry( "Scaling" );
    isLooping = cfg.readBoolEntry( "Looping" );
    isShuffled = cfg.readBoolEntry( "Shuffle" );
    usePlaylist = cfg.readBoolEntry( "UsePlayList" );
    isPlaying = FALSE;
    isPaused = FALSE;
    curPosition = 0;
    curLength = 0;
    curView = 'l';
}


void MediaPlayerState::writeConfig( Config& cfg ) const {
    cfg.setGroup("Options");
    cfg.writeEntry("FullScreen", isFullscreen );
    cfg.writeEntry("Scaling", isScaled );
    cfg.writeEntry("Looping", isLooping );
    cfg.writeEntry("Shuffle", isShuffled );
    cfg.writeEntry("UsePlayList", usePlaylist );
}


struct MediaPlayerPlugin {
#ifndef QT_NO_COMPONENT
    QLibrary *library;
#endif
    MediaPlayerPluginInterface *iface;
    MediaPlayerDecoder *decoder;
    MediaPlayerEncoder *encoder;
};


static QValueList<MediaPlayerPlugin> pluginList;


// Find the first decoder which supports this type of file
MediaPlayerDecoder *MediaPlayerState::newDecoder( const QString& file ) {
    MediaPlayerDecoder *tmpDecoder = NULL;
    QValueList<MediaPlayerPlugin>::Iterator it;
    for ( it = pluginList.begin(); it != pluginList.end(); ++it ) {
  if ( (*it).decoder->isFileSupported( file ) ) {
      tmpDecoder = (*it).decoder;
      break;
  }
    }
    return decoder = tmpDecoder; 
}


MediaPlayerDecoder *MediaPlayerState::curDecoder() {
    return decoder;
}


// ### hack to get true sample count
MediaPlayerDecoder *MediaPlayerState::libMpeg3Decoder() {
    return libmpeg3decoder;
}

// ### hack to get true sample count
// MediaPlayerDecoder *MediaPlayerState::libWavDecoder() {
//     return libwavdecoder;
// }

void MediaPlayerState::loadPlugins() {

#ifndef QT_NO_COMPONENT
    QValueList<MediaPlayerPlugin>::Iterator mit;
    for ( mit = pluginList.begin(); mit != pluginList.end(); ++mit ) {
  (*mit).iface->release();
  (*mit).library->unload();
  delete (*mit).library;
    }
    pluginList.clear();

    QString path = QPEApplication::qpeDir() + "/plugins/codecs";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
  MediaPlayerPluginInterface *iface = 0;
  QLibrary *lib = new QLibrary( path + "/" + *it );

  MediaPlayerDebug(( "querying: %s", QString( path + "/" + *it ).latin1() ));

  if ( lib->queryInterface( IID_MediaPlayerPlugin, (QUnknownInterface**)&iface ) == QS_OK ) {

      MediaPlayerDebug(( "loading: %s", QString( path + "/" + *it ).latin1() ));

      MediaPlayerPlugin plugin;
      plugin.library = lib;
      plugin.iface = iface;
      plugin.decoder = plugin.iface->decoder();
      plugin.encoder = plugin.iface->encoder();
      pluginList.append( plugin );

      // ### hack to get true sample count
      if ( plugin.decoder->pluginName() == QString("LibMpeg3Plugin") )
    libmpeg3decoder = plugin.decoder;

  } else {
      delete lib;
  }
    }
#else
    pluginList.clear();
    
    MediaPlayerPlugin plugin0;
    plugin0.iface = new LibMpeg3PluginImpl;
    plugin0.decoder = plugin0.iface->decoder();
    plugin0.encoder = plugin0.iface->encoder();
    pluginList.append( plugin0 );

    MediaPlayerPlugin plugin1;
    plugin1.iface = new LibMadPluginImpl;
    plugin1.decoder = plugin1.iface->decoder();
    plugin1.encoder = plugin1.iface->encoder();
    pluginList.append( plugin1 );

    MediaPlayerPlugin plugin2;
    plugin2.iface = new WavPluginImpl;
    plugin2.decoder = plugin2.iface->decoder();
    plugin2.encoder = plugin2.iface->encoder();
    pluginList.append( plugin2 );
#endif

    if ( pluginList.count() ) 
  MediaPlayerDebug(( "%i decoders found", pluginList.count() ));
    else
  MediaPlayerDebug(( "No decoders found" ));
}

