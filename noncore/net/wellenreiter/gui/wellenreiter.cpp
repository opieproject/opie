/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
***********************************************************************/

#include "wellenreiter.h"
#include "scanlistitem.h"

#include <qpushbutton.h>

Wellenreiter::Wellenreiter( QWidget* parent, const char* name, WFlags fl )
    : WellenreiterBase( parent, name, fl )
{

    connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );

}

Wellenreiter::~Wellenreiter()
{
    // no need to delete child widgets, Qt does it all for us
}

void Wellenreiter::buttonClicked()
{

    // FIXME: communicate with daemon and set button text according to state

    button->setText( "Stop Scanning" );

    // add some icons, so that we can see if this works

    new MScanListItem( netview, "managed", "MyNet", "04:00:20:EF:A6:43", true, 6, 80 );
    new MScanListItem( netview, "adhoc", "YourNet", "40:03:A3:E7:56:22", false, 11, 30 );

}
