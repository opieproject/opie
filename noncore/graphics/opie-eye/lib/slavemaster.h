#ifndef OPIE_EYE_SLAVE_MASTER_H
#define OPIE_EYE_SLAVE_MASTER_H

#include <iface/dirlister.h>
#include <iface/slaveiface.h>

#include <qobject.h>
#include <qstring.h>
#include <qsize.h>

class SlaveMaster : public QObject {
    Q_OBJECT
    typedef QValueList<ImageInfo> ImageInfos;
    typedef QValueList<PixmapInfo> PixmapInfos;
public:
    static SlaveMaster *self();

    void thumbInfo( const QString& );
    void imageInfo( const QString& );
    void thumbNail( const QString&, int w, int h );
    QImage  image( const QString&, PDirLister::Factor, int );
signals:
    void sig_start();
    void sig_end();

    void sig_thumbInfo( const QString&, const QString& );
    void sig_fullInfo( const QString&, const QString& );
    void sig_thumbNail( const QString&, const QPixmap& );
private slots:
    void recieve( const QCString&, const QByteArray& );
    void slotTimerStart();
private:
    SlaveMaster();
    ~SlaveMaster();
    static SlaveMaster *m_master;
    bool m_started : 1;
    QStringList m_inThumbInfo;
    QStringList m_inImageInfo;
    PixmapInfos m_inThumbNail;
};


#endif
