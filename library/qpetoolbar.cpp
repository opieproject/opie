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

#include "qpetoolbar.h"
#include "qpemenubar.h"
#include <qtoolbutton.h>

/*!
  \class QPEToolBar qpemenubar.h
  \brief The QPEToolBar class is obsolete.  Use QToolBar instead.

  \obsolete

  The QPEToolBar class is obsolete.  Use QToolBar instead.

  \sa QToolBar
 */

/*!
  Constructs a QPEToolBar just as you would construct
  a QToolBar, passing \a parent and \a name.
 */
QPEToolBar::QPEToolBar( QMainWindow *parent, const char *name )
    : QToolBar( parent, name )
{
}

/*!
  \internal
*/
void QPEToolBar::childEvent( QChildEvent *e )
{
    QToolBar::childEvent( e );
}

