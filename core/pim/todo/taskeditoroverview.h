/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef TASKEDITOROVERVIEW_H
#define TASKEDITOROVERVIEW_H

#include <opie/otodo.h>

#include <qpixmap.h>
#include <qwidget.h>

class CategorySelect;
class QCheckBox;
class QComboBox;
class QMultiLineEdit;

class TaskEditorOverView : public QWidget
{ 
    Q_OBJECT

public:
    TaskEditorOverView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TaskEditorOverView();

    QComboBox      *cmbDesc;
    QComboBox      *cmbPriority;
    CategorySelect *cmbCategory;
    QCheckBox      *ckbRecurrence;
    QMultiLineEdit *mleNotes;

    void load( const OTodo & );
    void save( OTodo & );

signals:
    void recurranceEnabled( bool );

protected:
    QPixmap m_pic_priority[ 5 ];

protected slots:
    void slotRecClicked();
};

#endif // TASKEDITOROVERVIEW_H
