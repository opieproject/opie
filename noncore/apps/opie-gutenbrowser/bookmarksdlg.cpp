/***************************************************************************
                          bookmarksdlg.cpp  -  description
                             -------------------
    begin                : Wed Dec 1 1999
    copyright            : (C) 1999 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// QMessageBox::message( "Note", listItemNumber );
#include "bookmarksdlg.h"


bookMarksDlg::bookMarksDlg(QWidget *parent,QString name) : QDialog(parent,name,true)
{
//	initDialog();

}

bookMarksDlg::~bookMarksDlg(){
}

void bookMarksDlg::doIt()
{


	accept();
}
