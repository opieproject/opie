/**
 * \file multiauthcommon.h
 * \brief Objects and functions for Opie multiauth framework
 * \author Clément Séveillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-        
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB. 
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef MULTIAUTHCOMMON_H
#define MULTIAUTHCOMMON_H


/* OwnerDialog stuff */
#include <qpe/global.h>
#include <qpe/contact.h>
#include <qtextview.h>
#include <qdialog.h>

class SecOwnerDlg : public QDialog
{
    Q_OBJECT
    public:

        SecOwnerDlg( QWidget *parent, const char * name, Contact c,
                     bool modal, bool fullscreen);

        void resizeEvent( QResizeEvent * );
        bool eventFilter(QObject *o, QEvent *e);
        void mousePressEvent( QMouseEvent * );

    private:
        QTextView *tv;
};

int runPlugins();

#endif // MULTIAUTHCOMMON_H
