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

#include "doctab.h"

#include <qpe/global.h>
#include <qpe/fontmanager.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/qpedialog.h>
#include <qpe/qpeapplication.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qlabel.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qslider.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qdir.h>
#if QT_VERSION >= 300
#include <qstylefactory.h>
#endif

#include <stdlib.h>


DocTabSettings::DocTabSettings( QWidget* parent, const char* name, WFlags fl )
		: DocTabSettingsBase( parent, name, TRUE, fl )
{
	dl = new QPEDialogListener(this);
	reset();
}

DocTabSettings::~DocTabSettings()
{}

void DocTabSettings::accept()
{
	applyDocTab();
	QDialog::accept();
}

void DocTabSettings::applyDocTab()
{
    Config cfg( "Launcher" );
    cfg.setGroup( "DocTab" );
    cfg.writeEntry( "Enable", yes->isChecked() );
    cfg.write();
}


void DocTabSettings::reject()
{
	reset();
	QDialog::reject();
}

void DocTabSettings::reset()
{
}

QString DocTabSettings::actualDocTab;

void DocTabSettings::done(int r)
{
	QDialog::done(r);
	close();
}
