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

#include <qdialog.h>

#include "qpeapplication.h"


// Allow access to nornally protected accept and reject functions
class HackedPrivateQDialog : public QDialog{
public:
    void accept() { QDialog::accept();}
    void reject() { QDialog::reject();}
};


/**
 * \brief This is the only c'tor.
 *
 * The parent of this Listener is the Dialog you pass. This means once
 * the dialog is deleted this listener will be deleted too.
 * This Listener listens on QPEApplication::appMessage signal and implements
 * accept() and reject().
 *
 * \code
 *	QDialog *dialog = new YourDialog();
	(void)new QPEDialogListener(dialog);
	if( QPEApplication::execDialog(dialog) == QDialog::Accept ){
	    // do some stuff
	}
	delete dialog;

 * \endcode
 *
 * @param di The dialog to handle
 */
QPEDialogListener::QPEDialogListener(QDialog *di ) : QObject(di)
{
    dialog = di;
    connect(qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
	    this, SLOT(appMessage(const QCString&,const QByteArray&)) );
}


/**
 * d'tor
 */
QPEDialogListener::~QPEDialogListener() {}

/**
 * \internal
 */
void QPEDialogListener::appMessage( const QCString &msg, const QByteArray & )
{
    if (!dialog)
	return;

    HackedPrivateQDialog *hackedDialog = static_cast<HackedPrivateQDialog*>(dialog);

    if (msg == "accept()") {
	hackedDialog->accept();
    } else if (msg == "reject()") {
	hackedDialog->reject();
    }
}
