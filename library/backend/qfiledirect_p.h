/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** Licensees holding valid Qtopia Developer license may use this
** file in accordance with the Qtopia Developer License Agreement
** provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** email sales@trolltech.com for information about Qtopia License
** Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QFILE_DIRECT_H
#define QFILE_DIRECT_H
#include <qfile.h>
#include <qpe/qpcglobal.h>

class QPC_EXPORT QFileDirect : public QFile
{
public:
	QFileDirect() : QFile() { }
    QFileDirect( const QString &name ) : QFile(name) { }

	FILE *directHandle() { return fh; }
};

#endif