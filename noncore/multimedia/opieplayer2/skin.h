#ifndef SKIN_H
#define SKIN_H

#include <qstring.h>
#include <qimage.h>
#include <qmap.h>
#include <qdict.h>

#include "mediawidget.h"
#include "threadutil.h"
#include "singleton.h"

class Skin
{
public:
    Skin( const QString &name, const QString &fileNameInfix );
    Skin( const QString &fileNameInfix );

    void preload( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount );

    QImage backgroundImage() const;
    QImage buttonUpImage() const;
    QImage buttonDownImage() const;

    QImage buttonMask( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount ) const;

    QImage buttonMaskImage( const QString &fileName ) const;

    static QString defaultSkinName();

private:
    void init( const QString &name );

    void addButtonToMask( int tag, const QImage &maskImage ) const;

    QString m_fileNameInfix;
    QString m_skinPath;

    typedef QMap<QString, QImage> ButtonMaskImageMap;

    mutable QImage m_backgroundImage;
    mutable QImage m_buttonUpImage;
    mutable QImage m_buttonDownImage;
    mutable QImage m_buttonMask;
    mutable ButtonMaskImageMap m_buttonMasks;

    Skin( const Skin & );
    Skin &operator=( const Skin & );
};

class SkinCache : public Singleton<SkinCache>
{
public:
    SkinCache();

    QImage loadImage( const QString &name );

private:
    typedef QDict<QImage> ImageCache;

    ImageCache m_cache;

    ThreadUtil::Mutex m_cacheGuard;
};

class SkinLoader : public ThreadUtil::Thread
{
public:
    SkinLoader();

    void schedule( const QString &skinName, const QString &fileNameInfix, 
                   const MediaWidget::SkinButtonInfo *skinButtonInfo, const uint buttonCount );

protected:
    virtual void run();

private:
    struct Info
    {
        Info() : skinButtonInfo( 0 ), buttonCount( 0 ) {}
        Info( const QString &_skinName, const QString &_fileNameInfix, 
              const MediaWidget::SkinButtonInfo *_skinButtonInfo, const uint _buttonCount )
            : skinName( _skinName ), fileNameInfix( _fileNameInfix ), 
              skinButtonInfo( _skinButtonInfo ), buttonCount( _buttonCount )
        {}

        const QString skinName;
        const QString fileNameInfix;
        const MediaWidget::SkinButtonInfo *skinButtonInfo;
        const uint buttonCount;
    };
    typedef QValueList<Info> InfoList;

    void load( const Info &nfo );

    InfoList pendingSkins;
    ThreadUtil::Mutex guard;
};

#endif // SKIN_H
/* vim: et sw=4 ts=4
 */
