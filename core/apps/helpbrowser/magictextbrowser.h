/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
** Copyright (C) 2003 zecke
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
#ifndef HELP_BROWSER_MAGIC_TEXT_BROWSER_H
#define HELP_BROWSER_MAGIC_TEXT_BROWSER_H

#include <qtextbrowser.h>

class MagicTextBrowser : public QTextBrowser {
public:
    MagicTextBrowser(QWidget* parent);

    void setSource( const QString& source );

    /** honor QPE Attributes */
    bool magicQpe(const QString& source, const QString& name);
    /** honor OPIE Attributes */
    bool magicOpe(const QString& source, const QString& name );
private:

    QString generateQpe(const QString& name) const;
    QString generateOpe(const QString& name) const;
};

#endif
