/***************************************************************************
   application:             : ODict

   begin                    : December 2002
   copyright                : ( C ) 2002, 2003 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "configdlg.h"

#include <qdialog.h>
#include <qpe/config.h>

ConfigDlg::ConfigDlg(QWidget *parent, const char *name) : QDialog(parent, name)
{
	this->show();
	setCaption( tr( "Options" ) );
}
