#ifndef SKIN_H
#define SKIN_H

#include <qstring.h>
#include <qimage.h>
#include <qmap.h>

#include "mediawidget.h"

class Skin
{
public:
    Skin( const QString &name, const QString &fileNameInfix );

    void preload( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount, const QSize &buttonAreaSize );

    QImage backgroundImage() const;
    QImage buttonUpImage() const;
    QImage buttonDownImage() const;

    QImage buttonMask( const MediaWidget::SkinButtonInfo *skinButtonInfo, uint buttonCount, const QSize &buttonAreaSize ) const;

    QImage buttonMaskImage( const QString &fileName ) const;

private:
    void addButtonToMask( int tag, const QImage &maskImage ) const;

    QString m_name;
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

#endif // SKIN_H
/* vim: et sw=4 ts=4
 */
