/****************************************************************************
** $Id: qsvgdevice_p.h,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Definition of the QSvgDevice class
**
** Created : 20001024
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the xml module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#ifndef QSVGDEVICE_H
#define QSVGDEVICE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QPicture class. This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "qpaintdevice.h"
#include "qrect.h"
#include "qdom.h"
#endif // QT_H

#if !defined(QT_MODULE_XML) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_XML )
#define QM_EXPORT_SVG
#else
#define QM_EXPORT_SVG Q_EXPORT
#endif

#ifndef QT_NO_SVG

class QPainter;
class QDomNode;
class QDomNamedNodeMap;
struct QSvgDeviceState;
class QSvgDevicePrivate;

class QM_EXPORT_SVG QSvgDevice : public QPaintDevice
{
public:
    QSvgDevice();
    ~QSvgDevice();

    bool play( QPainter *p );

    QString toString() const;

    bool load( QIODevice *dev );
    bool save( QIODevice *dev );
    bool save( const QString &fileName );

    QRect boundingRect() const;
    void setBoundingRect( const QRect &r );

protected:
    virtual bool cmd ( int, QPainter*, QPDevCmdParam* );
    virtual int	 metric( int ) const;

private:
    // reading
    bool play( const QDomNode &node );
    void saveAttributes();
    void restoreAttributes();
    QColor parseColor( const QString &col );
    double parseLen( const QString &str, bool *ok=0, bool horiz=TRUE ) const;
    int lenToInt( const QDomNamedNodeMap &map, const QString &attr,
		  int def=0 ) const;
    void setStyleProperty( const QString &prop, const QString &val,
			   QPen *pen, QFont *font, int *talign );
    void setStyle( const QString &s );
    void setTransform( const QString &tr );
    void drawPath( const QString &data );

    // writing
    void applyStyle( QDomElement *e, int c ) const;
    void applyTransform( QDomElement *e ) const;

    // reading
    QRect brect;			// bounding rectangle
    QDomDocument doc;			// document tree
    QDomNode current;
    QPoint curPt;
    QSvgDeviceState *curr;
    QPainter *pt;			// used by play() et al

    // writing
    bool dirtyTransform, dirtyStyle;

    QSvgDevicePrivate *d;
};

inline QRect QSvgDevice::boundingRect() const
{
    return brect;
}

#endif // QT_NO_SVG

#endif // QSVGDEVICE_H
