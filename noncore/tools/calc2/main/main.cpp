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
#include <qpe/qpeapplication.h>
#include "calc.h"

int main (int argc, char **argv)
{
  QPEApplication a (argc, argv);

  calc mw;

  QPEApplication::setInputMethodHint (&mw, QPEApplication::AlwaysOff);
  mw.setCaption (calc::tr ("Calculator"));
  a.showMainWidget (&mw);

  return a.exec ();
}