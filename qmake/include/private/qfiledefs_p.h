/****************************************************************************
** $Id: qfiledefs_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Common macros and system include files for QFile, QFileInfo and QDir.
**
** Created : 930812
**
** Copyright (C) 1992-2001 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
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
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
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
**********************************************************************/

#ifndef QFILEDEFS_P_H
#define QFILEDEFS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qfileinfo*.cpp.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//
//

// Be sure to include qplatformdefs.h first!
struct QFileInfoCache
{
#if defined(Q_WS_WIN)
    QT_STATBUF st;
#else
    struct stat st;
#endif
};


#endif
