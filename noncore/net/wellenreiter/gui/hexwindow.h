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
**********************************************************************/

#ifndef HEXWINDOW_H
#define HEXWINDOW_H

#include <qvbox.h>

class QString;
class QMultiLineEdit;

class MHexWindow: public QVBox
{

  public:
    MHexWindow( QWidget * parent = 0, const char * name = "MHexWindow", WFlags f = 0 );
  
    void log( QString text );
    
  protected:
    QMultiLineEdit* ledit;

};

#endif

