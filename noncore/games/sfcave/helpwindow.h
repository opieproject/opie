/***************************************************************************
                          helpwindow.h  -  description
                             -------------------
    begin                : Sun Sep 8 2002
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

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <qdialog.h>

/**
  *@author Andy Qua
  */

class HelpWindow : public QDialog
{
public: 
    HelpWindow( QWidget *parent = 0, const char *name = 0, bool modal = true, WFlags flags = 0 );
	~HelpWindow();
};

#endif
