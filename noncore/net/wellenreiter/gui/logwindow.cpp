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

#include "logwindow.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qmultilineedit.h>
#include <qdatetime.h>

MLogWindow* MLogWindow::_instance;

MLogWindow::MLogWindow( QWidget * parent, const char * name, WFlags f )
           :QVBox( parent, name, f )
{
    ledit = new QMultiLineEdit( this );
    ledit->setReadOnly( true );

    MLogWindow::_instance = this;
}


void MLogWindow::log( QString text )
{
    QTime time = QTime::currentTime();
    QString line;
    line.sprintf( "[%s] %s\n", (const char*) time.toString(), (const char*) text );
    int col;
    int row;
    ledit->getCursorPosition( &col, &row );
    ledit->insertAt( line, col, row );
    odebug << line << oendl; 
}


void MLogWindow::clear()
{
    ledit->clear();
}


const QString MLogWindow::getLog() const
{
    return ledit->text();
}
