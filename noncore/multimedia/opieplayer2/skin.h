/*
   Copyright (C) 2002 Simon Hausmann <simon@lst.de>
             (C) 2002 Max Reiss <harlekin@handhelds.org>
             (C) 2002 L. Potter <ljp@llornkcor.com>
             (C) 2002 Holger Freyther <zecke@handhelds.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef SKIN_H
#define SKIN_H

#include <qstring.h>
#include <qimage.h>
#include <qmap.h>
#include <qdict.h>

#include "mediawidget.h"
#include "threadutil.h"
#include "singleton.h"

struct SkinData;

class Skin
{
public:
    Skin( const QString &name, const QString &fileNameInfix );
    Skin( const QString &fileNameInfix );
    ~Skin();

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

    SkinData *d;

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
