#include "slavemaster.h"

#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>

#include <qcopchannel_qws.h>
#include <qtimer.h>

QDataStream & operator << (QDataStream & str, bool b)
{
  str << Q_INT8(b);
  return str;
}
QDataStream & operator >> (QDataStream & str, bool & b)
{
  Q_INT8 l;
  str >> l;
  b = bool(l);
  return str;
}
QDataStream &operator<<( QDataStream& s, const PixmapInfo& inf) {
    return s << inf.file << inf.pixmap << inf.width << inf.height;
}
QDataStream &operator>>( QDataStream& s, PixmapInfo& inf ) {
    s >> inf.file >> inf.pixmap >> inf.width >> inf.height;
    return s;
}
QDataStream &operator<<( QDataStream& s, const ImageInfo& i) {
    return s << i.kind << i.file << i.info;
}
QDataStream &operator>>( QDataStream& s, ImageInfo& i ) {
    s >> i.kind >> i.file >> i.info;
    return s;
}



SlaveMaster* SlaveMaster::m_master = 0;

SlaveMaster::SlaveMaster()
    : m_started( false )
{
    QCopChannel *chan= new QCopChannel( "QPE/opie-eye",this );
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&)),
            this, SLOT(recieve(const QCString&,const QByteArray&)) );
}

SlaveMaster::~SlaveMaster() {
}

SlaveMaster* SlaveMaster::self() {
    if ( !m_master )
        m_master = new SlaveMaster;
    return m_master;
}

void SlaveMaster::thumbInfo( const QString& str) {
    m_inThumbInfo.append( str );

    if ( !m_started ) {
        QTimer::singleShot( 0, this, SLOT(slotTimerStart()));
        m_started = true;
    }
}

void SlaveMaster::imageInfo( const QString& str ) {
    m_inImageInfo.append( str );
    if ( !m_started ) {
        QTimer::singleShot( 0, this, SLOT(slotTimerStart()));
        m_started = true;
    }
}

void SlaveMaster::thumbNail( const QString& str, int w, int h ) {
    if ( str.isEmpty() ) {
        qWarning( "Asking for empty nail" );
        return;
    }
    qWarning( "Asking for thumbNail in size %d %d" + str, w,h );
    PixmapInfo item;
    item.file = str; item.width = w; item.height = h;
    item.pixmap = QPixmap();
    m_inThumbNail.append( item );

    if ( !m_started ) {
        QTimer::singleShot( 0, this, SLOT(slotTimerStart()));
        m_started = true;
    }
}


void SlaveMaster::recieve( const QCString& str, const QByteArray& at) {

    ImageInfos infos;
    PixmapInfos pixinfos;

    QDataStream stream( at, IO_ReadOnly );
    if ( str == "pixmapsHandled(PixmapList)" )
        stream >> pixinfos;
    else if ( str == "pixmapsHandled(StringList)" )
        stream >> infos;

    qWarning( "PixInfos %d", pixinfos.count() );

    bool got_data = ( !infos.isEmpty() || !pixinfos.isEmpty() );
    if ( got_data ) {
        emit sig_start();
        for ( ImageInfos::Iterator _it = infos.begin(); _it != infos.end(); ++_it ) {
            if (  (*_it).kind )
                emit  sig_fullInfo( (*_it).file, (*_it).info );
            else
                emit sig_thumbInfo( (*_it).file, (*_it).info );
        }

        for ( PixmapInfos::Iterator it = pixinfos.begin(); it != pixinfos.end(); ++it )
            emit sig_thumbNail(  (*it).file, (*it).pixmap );
        emit sig_end();
    }
}

void SlaveMaster::slotTimerStart() {
    m_started = false;

    if ( !m_inThumbInfo.isEmpty() ) {
        QCopEnvelope env("QPE/opie-eye_slave", "thumbInfos(QStringList)" );
        env << m_inThumbInfo;
    }
    if ( !m_inImageInfo.isEmpty() ) {
        QCopEnvelope env("QPE/opie-eye_slave", "fullInfos(QStringList)" );
        env << m_inImageInfo;
    }
    if ( !m_inThumbNail.isEmpty() ) {
        QCopEnvelope env("QPE/opie-eye_slave", "pixmapInfos(PixmapInfos)" );
        env << m_inThumbNail;
    }


    m_inThumbInfo.clear();
    m_inImageInfo.clear();
    m_inThumbNail.clear();
}

QImage SlaveMaster::image( const QString& str, PDirLister::Factor, int ) {
    return QImage();
}
