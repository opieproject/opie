/*
 * GPLv2
 */


#ifndef SLAVE_RECEIVER_H
#define SLAVE_RECEIVER_H

/**
 * Receive Requests
 */

#include <iface/slaveiface.h>

#include <qobject.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qpixmap.h>



typedef QValueList<PixmapInfo> PixmapList;
typedef QValueList<ImageInfo>  StringList;

class QTimer;
class QSocket;
class SlaveReciever : public QObject {
    Q_OBJECT

    friend QDataStream &operator<<( QDataStream&, const PixmapInfo& );
    friend QDataStream &operator>>( QDataStream&, PixmapInfo& );
    friend QDataStream &operator<<( QDataStream&, const ImageInfo& );
    friend QDataStream &operator>>( QDataStream&, ImageInfo );
public:

    enum Job { ImageInfoJob,  FullImageInfoJob, ThumbNailJob };
    SlaveReciever( QObject* parent );
    ~SlaveReciever();

public slots:
    void recieveAnswer( const QCString&, const QByteArray& );
public:
    PixmapList outPix()const;
    StringList outInf()const;

private slots:
    void slotSend();
    void slotImageInfo();
    void slotThumbNail();
private:
    QTimer *m_inf, *m_pix, *m_out;
    StringList m_inList, m_outList;
    PixmapList m_inPix, m_outPix;
private:
    int m_refs;
};


#endif
