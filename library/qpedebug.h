/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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
#ifndef QPEDEBUG_H
#define QPEDEBUG_H

#define QPE_DEBUGTIMEDESC(X) qpe_debugTime( __FILE__, __LINE__, X );
#define QPE_DEBUGTIME qpe_debugTime( __FILE__, __LINE__ );

void qpe_debugTime( const char *file, int line, const char *desc=0 );

#endif

