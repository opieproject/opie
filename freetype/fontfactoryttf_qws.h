/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/


#ifndef FONTFACTORY_FT_H
#define FONTFACTORY_FT_H

#include <qfontmanager_qws.h>
#if QT_VERSION >= 300
# include <private/qfontdata_p.h>
#else
# include "qfontdata_p.h"
#endif


#ifdef QT_NO_FREETYPE

extern "C" {
#include <freetype/freetype.h>
}

// ascent, descent, width(ch), width(string), maxwidth?
// leftbearing, rightbearing, minleftbearing,minrightbearing
// leading

class QFontFactoryFT : public QFontFactory {

public:

    QFontFactoryFT();
    virtual ~QFontFactoryFT();

    QRenderedFont * get(const QFontDef &,QDiskFont *);
    virtual void load(QDiskFont *) const;
    virtual QString name();

private:

    friend class QRenderedFontFT;
    FT_Library library;
};

#endif // QT_NO_FREETYPE

#endif // QFONTFACTORY_FT_H

