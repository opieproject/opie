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

#include "qpedialog.h"
#include "qpeapplication.h"

#define protected public
#include <qdialog.h>

QPEDialogListener::QPEDialogListener(QDialog *di ) : QObject(di)
{
    dialog = di;
    connect(qApp, SIGNAL(appMessage(const QCString&, const QByteArray&)),
	    this, SLOT(appMessage(const QCString&, const QByteArray&)) );
}

QPEDialogListener::~QPEDialogListener() {}

void QPEDialogListener::appMessage( const QCString &msg, const QByteArray &data )
{
    if (!dialog) 
	return;
    if (msg == "accept()") {
	dialog->accept();
    } else if (msg == "reject()") {
	dialog->reject();
    }
}
