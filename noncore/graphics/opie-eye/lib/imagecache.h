/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_IMAGE_CACHE_H
#define PHUNK_IMAGE_CACHE_H

#include <qimage.h>
#include <qpixmap.h>
#include <qcache.h>


class PImageCache : public QCache<QImage> {
private:
    PImageCache();
    ~PImageCache();

public:
    static PImageCache *self();
    QImage* cachedImage( const QString& path, int orientation = 3,  int max = 0); //const;
    void insertImage(  const QString& path, const QImage &, int orien = 3, int max = 0);
    void insertImage(  const QString& path, const QImage *, int orien=3, int max = 0 );
};


class PPixmapCache : public  QCache<QPixmap> {
private:
    PPixmapCache();
    ~PPixmapCache();
public:
    static PPixmapCache *self();
    QPixmap* cachedImage(  const QString&  path, int width, int height );
    void insertImage( const QString& path, const QPixmap &, int width,  int height );
    void insertImage( const QString& path, const QPixmap *, int width, int height );
};

inline void PPixmapCache::insertImage( const QString& path, const QPixmap& p, int width, int height ) {
    insertImage( path,  new QPixmap( p ), width, height );
}

inline void PImageCache::insertImage( const QString& path, const QImage& p, int width, int height ) {
    insertImage( path, new QImage( p ), width, height );
}

#endif
