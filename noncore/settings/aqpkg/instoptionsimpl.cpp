/***************************************************************************
                          instoptionsimpl.cpp  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef QWS
#include <qpe/config.h>
#endif

#include <qdialog.h>
#include <qcheckbox.h>

#include "instoptionsimpl.h"
#include "ipkg.h"
#include "global.h"

InstallOptionsDlgImpl::InstallOptionsDlgImpl( int flags, QWidget * parent, const char* name, bool modal, WFlags fl )
    : InstallOptionsDlg( parent, name, modal, fl )
{
    if ( flags & FORCE_DEPENDS )
    	forceDepends->setChecked( true );
    if ( flags & FORCE_REINSTALL )
    	forceReinstall->setChecked( true );
    if ( flags & FORCE_REMOVE )
    	forceRemove->setChecked( true );
    if ( flags & FORCE_OVERWRITE )
    	forceOverwrite->setChecked( true );
    if ( flags & MAKE_LINKS )
    	makeLinks->setChecked( true );

  	showMaximized();

}

InstallOptionsDlgImpl::~InstallOptionsDlgImpl()
{
}
