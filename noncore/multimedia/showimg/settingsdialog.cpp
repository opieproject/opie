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

#include "settingsdialog.h"
#include <qslider.h>
#include <qlabel.h>
#include <qcheckbox.h>

SettingsDialog::SettingsDialog( QWidget *parent, const char *name, bool modal, WFlags f )
    : SettingsDialogBase( parent, name, modal, f )
{
    connect( delaySlider, SIGNAL(valueChanged(int)), this, SLOT(delayChanged(int)) );
}

void SettingsDialog::setDelay( int d )
{
    delaySlider->setValue( d );
    delayChanged( d );
}

int SettingsDialog::delay() const
{
    return delaySlider->value();
}

void SettingsDialog::setRepeat( bool r )
{
    repeatCheck->setChecked( r );
}

bool SettingsDialog::repeat() const
{
    return repeatCheck->isChecked();
}

void SettingsDialog::delayChanged( int d )
{
    delayText->setText( QString::number( d ) + " s" );
}

