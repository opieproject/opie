/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
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

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <qvbox.h>

class QString;
class QMultiLineEdit;

class MLogWindow: public QVBox
{

  public:
    MLogWindow( QWidget * parent = 0, const char * name = "MLogWindow", WFlags f = 0 );

    void log( QString text );
    const QString getLog() const;
    void clear();

    static MLogWindow* logwindow() { return MLogWindow::_instance; };

  protected:
    QMultiLineEdit* ledit;

  private:
    static MLogWindow* _instance;

};

#endif

