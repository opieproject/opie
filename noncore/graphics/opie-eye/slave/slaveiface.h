/*
 * GPLv2
 */

#ifndef P_SLAVE_INTER_FACE_H
#define P_SLAVE_INTER_FACE_H

#include <qfileinfo.h>
#include <qstringlist.h>
#include <qmap.h>

/**
 * @short The slave worker Interface for generating Preview + Image Info + Full
 * IMage Info
 */
class QPixmap;
class SlaveInterface {
public:
    SlaveInterface(const QStringList& imageformats);
    virtual ~SlaveInterface();

    QStringList imageFormats()const;
    bool supports( const QString& )const;
    virtual QString iconViewName(const QString&) = 0;
    virtual QString fullImageInfo(const QString& )= 0;
    virtual QPixmap pixmap( const QString&,  int width,  int height ) = 0;
private:
    QStringList m_list;
};

inline bool SlaveInterface::supports( const QString& str)const {
    return m_list.contains( QFileInfo( str ).extension(false).lower() );
}

typedef SlaveInterface* (*phunkSlaveCreateFunc )();
typedef QMap<QString,phunkSlaveCreateFunc> SlaveMap;

typedef QMap<QString, SlaveInterface*> SlaveObjects;

SlaveMap* slaveMap();
SlaveObjects* slaveObjects();



#define PHUNK_VIEW_INTERFACE( NAME, IMPL ) \
    static SlaveInterface *create_ ## IMPL() { \
        return new IMPL(); \
    }                                          \
    static SlaveMap::Iterator dummy_ ## IMPL = slaveMap()->insert( NAME, create_ ## IMPL );



#endif
