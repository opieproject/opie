/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#ifndef QPETOOLBAR_H
#define QPETOOLBAR_H

#include <qtoolbar.h>

class QMainWindow;

class QPEToolBar : public QToolBar
{
    Q_OBJECT
public:
    QPEToolBar( QMainWindow *parent = 0, const char *name = 0 );

protected:
    virtual void childEvent( QChildEvent *e );
};

#endif

