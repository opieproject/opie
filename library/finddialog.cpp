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

// WARNING: Do *NOT* define this yourself. The SL5xxx from SHARP does NOT
//      have this class.
#define QTOPIA_INTERNAL_FD

#include "finddialog.h"
#include "findwidget_p.h"

#include <qlayout.h>
#include <qpushbutton.h>

FindDialog::FindDialog( const QString &appName, QWidget *parent,
                        const char *name, bool modal )
    : QDialog( parent, name, modal )
{
    QVBoxLayout *vb;
    vb = new QVBoxLayout( this );
    fw = new FindWidget( appName, this, "Find Widget" );
    vb->addWidget( fw );
    QObject::connect( fw, SIGNAL(signalFindClicked(const QString&,
						   bool,bool,int)),
		      this, SIGNAL(signalFindClicked(const QString&,
						     bool,bool,int)) );
    QObject::connect( fw, SIGNAL(signalFindClicked(const QString&,const QDate&,
						   bool,bool,int)),
		      this, SIGNAL(signalFindClicked(const QString&,
						     const QDate&,bool,bool,int)) );
    d = 0;
}

FindDialog::~FindDialog()
{
}

QString FindDialog::findText() const
{
    return fw->findText();
}

void FindDialog::setUseDate( bool show )
{
    fw->setUseDate( show );
}

void FindDialog::setDate( const QDate &dt )
{
    fw->setDate( dt );
}

void FindDialog::slotNotFound()
{
    fw->slotNotFound();
}

void FindDialog::slotWrapAround()
{
    fw->slotWrapAround();
}
