/****************************************************************************
** $Id: qsharedmemory_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Includes system files for shared memory
**
** Created : 020124
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_qws.cpp and qgfxvnc_qws.cpp.  This header file may 
// change from version to version without notice, or even be removed.
//
//

#if !defined(QT_QSHM_H)
#define QT_QSHM_H

#include <qstring.h>

#if !defined (QT_QWS_NO_SHM)

#include <sys/types.h>
#include <sys/ipc.h>

class QSharedMemory {
public:
	QSharedMemory(){};
	QSharedMemory(int, QString, char c = 'Q');
	~QSharedMemory(){};

	bool create();
	void destroy();

	bool attach();
	void detach();

	void setPermissions(mode_t mode);
	int size();
	void * base() { return shmBase; };

private:
	void *shmBase;
	int shmSize;
	QString shmFile;
	char character;
#if defined(QT_POSIX_QSHM)
	int shmFD;
#else
	int shmId;
	key_t key;
	int idInitted;
#endif
};

#endif
#endif
