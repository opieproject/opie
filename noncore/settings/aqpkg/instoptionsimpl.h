/***************************************************************************
                          installoptionsimpl.h  -  description
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
#ifndef INSTALLOPTIONSIMPL_H
#define INSTALLOPTIONSIMPL_H

#include "instoptions.h"

class InstallOptionsDlgImpl : public InstallOptionsDlg
{
public:
    InstallOptionsDlgImpl( int flags, QWidget * parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    ~InstallOptionsDlgImpl();

protected:

private:
};

#endif
