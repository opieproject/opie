/*
 * GPLv2 zecke@handhelds.org
 */

#include "slavereciever.h"
#include "slaveiface.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

/* QT */
#include <qtimer.h>

static SlaveObjects* _slaveObj = 0;

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

/*
 * GUI sends no QPIxmap!!!
 */
QDataStream &operator>>( QDataStream& s, PixmapInfo& inf ) {
    s >> inf.file >> inf.width >> inf.height;
    owarn << "Recieved " << inf.file.latin1() << " " << inf.width << " " << inf.height << "" << oendl;
    return s;
}
QDataStream &operator<<( QDataStream& s, const ImageInfo& i) {
    return s << i.kind << i.file << i.info;
}
QDataStream &operator>>( QDataStream& s, ImageInfo& i ) {
    s >> i.kind >> i.file >> i.info;
    return s;
}



SlaveObjects* slaveObjects() {
    if ( !_slaveObj )
        _slaveObj = new SlaveObjects;
    return _slaveObj;
}

SlaveReciever::SlaveReciever( QObject* par)
    : QObject( par ), m_refs( 0 )
{
    m_inf = new QTimer(this);
    connect(m_inf,SIGNAL(timeout()),
            this, SLOT(slotImageInfo()));
    m_pix = new QTimer(this);
    connect(m_pix,SIGNAL(timeout()),
            this, SLOT(slotThumbNail()));

    m_out = new QTimer(this);
    connect(m_out,SIGNAL(timeout()),
            this, SLOT(slotSend()));

    SlaveObjects *obj = slaveObjects(); // won't be changed
    SlaveMap::Iterator it;
    SlaveMap* map = slaveMap(); // SlaveMap won't be changed during execution!!!
    for(it = map->begin(); it != map->end(); ++it ) {
        obj->insert( it.key(),  (*it.data())() );
    }
}

SlaveReciever::~SlaveReciever() {
}

void SlaveReciever::recieveAnswer( const QCString& string, const QByteArray& ar) {
    owarn << "String is " << string.data() << "" << oendl;
    QDataStream stream(ar, IO_ReadOnly );
    QStringList lst;
    static ImageInfo  inf;
    static PixmapInfo pix;

    if ( string == "thumbInfo(QString)" ) {
        stream >> inf.file;
        inf.kind = false;
        m_inList.append(inf);
    }else if ( string == "thumbInfos(QStringList)" ) {
        stream >> lst;
        for(QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
            owarn << "Adding thumbinfo for file "+ *it << oendl;
            inf.file = (*it);
            inf.kind = false;
            m_inList.append(inf);
        }
    }else if ( string == "fullInfo(QString)" ) {
        inf.kind = true;
        stream >> inf.file;
        m_inList.append(inf);
    }else if ( string == "fullInfos(QStringList)" ) {
        stream >> lst;
        for(QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
            owarn << "Adding fullInfo for"+ *it << oendl;
            inf.file = (*it);
            inf.kind = true;
            m_inList.append(inf);
        }
    }else if ( string == "pixmapInfo(QString,int,int)" ) {
        stream >> pix.file >> pix.width >> pix.height;
        m_inPix.append(pix);
    }else if ( string == "pixmapInfos(PixmapInfos)" ) {
        PixmapList list;
        stream >> list;
        for(PixmapList::Iterator it = list.begin(); it != list.end(); ++it ) {
            owarn << "Got " << (*it).width << " " << (*it).height << " " + (*it).file << oendl;
            m_inPix.append(*it);
        }
    }else if ( string == "refUp()" ) {
        m_refs++;
    }else if ( string == "refDown()" ) {
        m_refs--;
    }

    if (!m_inf->isActive() && !m_inList.isEmpty() )
        m_inf->start(5);

    if (!m_pix->isActive() && !m_inPix.isEmpty() )
        m_pix->start(5);

    if ( m_refs )
        QPEApplication::setKeepRunning();
    else
        qApp->quit();

}

PixmapList SlaveReciever::outPix()const {
    return m_outPix;
}

StringList SlaveReciever::outInf()const{
    return m_outList;
}

void SlaveReciever::slotImageInfo() {
    ImageInfo inf = m_inList.first();
    m_inList.remove( inf );

    static SlaveObjects::Iterator it;
    static SlaveObjects* map = slaveObjects(); // SlaveMap won't be changed during execution!!!
    for(it = map->begin(); it != map->end(); ++it ) {
        if( (*it)->supports(inf.file ) ) {
            /* full image info */
            if (inf.kind )
                inf.info = (*it)->fullImageInfo( inf.file );
            else
                inf.info = (*it)->iconViewName( inf.file );
            m_outList.append( inf );
            break;
        }
    }

    if (m_inList.isEmpty() )
        m_inf->stop();
    if (!m_out->isActive() && !m_outList.isEmpty() )
        m_out->start( 100 );
}

void SlaveReciever::slotThumbNail() {
    PixmapInfo inf = m_inPix.first();
    m_inPix.remove( inf );

    static SlaveObjects::Iterator it;
    static SlaveObjects* map = slaveObjects(); // SlaveMap won't be changed during execution!!!
    for(it = map->begin(); it != map->end(); ++it ) {
        SlaveInterface* iface = it.data();
        if( iface->supports(inf.file ) ) {
            /* pixmap */
            owarn << "Asking for thumbNail in size " << inf.width << " " << inf.height << " for "+inf.file << oendl;
            if (inf.width>256||inf.height>256) {
                owarn << "Scaling thumbnail to 64x64 'cause " << inf.width<<"x"<<inf.height<<
                    " may be nonsense";
                inf.pixmap = iface->pixmap(inf.file, 64, 64);
            }else {
                inf.pixmap = iface->pixmap(inf.file, inf.width, inf.height);
            }
            m_outPix.append( inf );
            break;
        }
    }



    if(m_inPix.isEmpty() )
        m_pix->stop();
    if(!m_out->isActive() && !m_outPix.isEmpty() )
        m_out->start(100);
}

void SlaveReciever::slotSend() {

    m_out->stop();

    owarn << "Sending " << outPix().count() << " " << outInf().count() << "" << oendl;
    /* queue it and send */
    /* if this ever gets a service introduce request queues
     * so we can differinatate between different clients
     */
    if (! m_outPix.isEmpty() ) {
        QCopEnvelope answer("QPE/opie-eye", "pixmapsHandled(PixmapList)" );
        answer << outPix();
        for ( PixmapList::Iterator it = m_outPix.begin();it!=m_outPix.end();++it ) {
            owarn << "Sending out " << (*it).file.latin1() << " " << (*it).width << " " << (*it).height << "" << oendl;
        }
    }
    if ( !m_outList.isEmpty() ) {
        QCopEnvelope answer("QPE/opie-eye", "pixmapsHandled(StringList)" );
        answer << outInf();
        for ( StringList::Iterator it = m_outList.begin();it!=m_outList.end();++it ) {
            owarn << "Sending out2 " + (*it).file << oendl;
        }
    }

    m_outList.clear();
    m_outPix.clear();
}

