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
#include <qobject.h>

#include "mediawidget.h"

struct SkinData;

class Skin
{
public:
    Skin( const QString &name, const QString &fileNameInfix );
    Skin( const QString &fileNameInfix );
    ~Skin();

    void preload( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount );

    QPixmap backgroundPixmap() const;
    QImage buttonUpImage() const;
    QImage buttonDownImage() const;

    QImage buttonMask( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount ) const;

    QImage buttonMaskImage( const QString &fileName ) const;

    static QString defaultSkinName();

private:
    void init( const QString &name );

    void addButtonToMask( int tag, const QImage &maskImage ) const;

    static QImage loadImage( const QString &fileName );

    QString m_fileNameInfix;
    QString m_skinPath;

    SkinData *d;

    Skin( const Skin & );
    Skin &operator=( const Skin & );
};

class SkinLoader : public QObject
{
    Q_OBJECT
public:
    SkinLoader();
    virtual ~SkinLoader();

    void schedule( const MediaWidget::GUIInfo &guiInfo );
    void schedule( const QString &skinName, const MediaWidget::GUIInfo &guiInfo );

    void start();

protected:
    virtual void timerEvent( QTimerEvent *ev );

private slots:
    void deleteMe();

private:
    struct Info : public MediaWidget::GUIInfo
    {
        Info() {}
        Info( const QString &_skinName, const MediaWidget::GUIInfo &guiInfo )
            : MediaWidget::GUIInfo( guiInfo ), skinName( _skinName )
        {}

        QString skinName;
    };
    typedef QValueList<Info> InfoList;

    class IncrementalLoader
    {
    public:
        enum LoaderResult { LoadingCompleted, MoreToCome };

        IncrementalLoader( const Info &info );

        LoaderResult loadStep();

    private:
        enum State { LoadBackgroundPixmap, LoadButtonUpImage, LoadButtonDownImage, LoadButtonMasks, LoadButtonMask };

        Skin m_skin;
        Info m_info;
        State m_currentState;
        uint m_currentButton;
    };

    InfoList pendingSkins;
    IncrementalLoader *m_currentLoader;
    int m_timerId;
};

#endif // SKIN_H
/* vim: et sw=4 ts=4
 */
