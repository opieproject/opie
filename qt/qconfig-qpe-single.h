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
#include "qconfig-qpe.h"
#define QT_NO_NETWORK
#define NO_CHECK

//#define QT_DEMO_SINGLE_FLOPPY


#ifdef QT_DEMO_SINGLE_FLOPPY 
// VGA16 is all we need
#define QT_NO_QWS_TRANSFORMED
#define QT_NO_QWS_MACH64
#define QT_NO_QWS_VOODOO3
#define QT_NO_QWS_MATROX
#define QT_NO_QWS_DEPTH_16
#define QT_NO_QWS_DEPTH_32
#define QT_NO_SOUND
#endif

#ifdef QT_NO_QWS_DEPTH_16
// Needed for mpegplayer on 16bpp
#define QT_NO_IMAGE_16_BIT
#endif
