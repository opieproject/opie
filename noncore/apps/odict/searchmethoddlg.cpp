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
#include "searchmethoddlg.h"

#include <qdialog.h>
#include <qpe/config.h>
#include <qlayout.h>

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>

SearchMethodDlg::SearchMethodDlg(QWidget *parent, const char *name, bool modal) : QDialog(parent, name, modal)
{
	setCaption( tr( "New Searchmethod" ) );

	itemName = "foo";
	showMaximized();
}

