/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "progresswidget.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qprogressbar.h>

ProgressWidget::ProgressWidget( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    QVBoxLayout *layout = new QVBoxLayout( this, 4, 4 );

    m_status = new QLabel( this );
    m_status->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    layout->addWidget( m_status );

    m_progress = new QProgressBar( this );
    layout->addWidget( m_progress );
}

ProgressWidget::~ProgressWidget()
{
}

void ProgressWidget::setSteps( int numsteps )
{
    m_progress->setTotalSteps( numsteps );
}

void ProgressWidget::setMessage( const QString &msg )
{
    m_status->setText( msg );
}

void ProgressWidget::update( int progress )
{
    m_progress->setProgress( progress );
}

