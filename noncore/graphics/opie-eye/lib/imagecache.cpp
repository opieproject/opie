/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#include <iface/dirview.h>
#include <iface/dirlister.h>

#include "imagecache.h"

namespace {
    PImageCache * _imgCache = 0;
    PPixmapCache* _pxmCache = 0;
}


PImageCache::PImageCache()
    : QCache<QImage>()
{
    /* just to set an initial value.. 4 big images */
    setMaxCost( (1024*1024*16)/8*4 );
}

PImageCache::~PImageCache() {
}

PImageCache* PImageCache::self() {
    if ( !_imgCache )
        _imgCache = new PImageCache;
    return _imgCache;
}

QImage* PImageCache::cachedImage( const QString& _path, int ori, int max ) {
    QString path = QString( "%1_%2:" ).arg( ori ).arg( max );
    path += _path;

    QImage* img = find( path );
    if ( !img ) {
//        img = currentView()->dirLister()->image( _path, PDirLister::Factor(ori),  max);
//        insertImage(  _path, img, ori, max );
        currentView()->dirLister()->image( _path, PDirLister::Factor( ori ), max );
    }


    return  img;
}

void PImageCache::insertImage(  const QString& _path,  const QImage* img, int ori, int max ) {
    QString path = QString("%1_%2:" ).arg( ori ).arg( max );
    path += _path;
    insert( path, img, (img->height()*img->width()*img->depth())/8 );
}


PPixmapCache::PPixmapCache() {
  /*
   * 20 64x64 16 bit images
   */
    setMaxCost( 64*64*QPixmap::defaultDepth()/8*20 );
}

PPixmapCache::~PPixmapCache() {
}

PPixmapCache* PPixmapCache::self() {
    if ( !_pxmCache )
        _pxmCache = new PPixmapCache;

    return _pxmCache;
}

QPixmap* PPixmapCache::cachedImage( const QString& _path, int width, int height ) {
    QString path = QString( "%1_%2:" ).arg( width ).arg( height );
    path += _path;

    QPixmap* pxm = find( path );



    return  pxm;
}

void PPixmapCache::insertImage( const QString& _path, const QPixmap* pix, int width, int height ) {
    QString path = QString("%1_%2:" ).arg( width ).arg( height );
    path += _path;
    insert( path, pix, (pix->height()*pix->width()*pix->depth())/8 );
}
