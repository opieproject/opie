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

#ifndef TASKEDITORADVANCED_H
#define TASKEDITORADVANCED_H

#include <qwidget.h>

class OTodo;
class QComboBox;
class QLabel;
class QMultiLineEdit;
class QToolButton;

/**
 * This is the implementation of the Opie Task Editor Advanced tab
 * it features the State!
 *                 MaintainerMode
 *                 Description
 */
class TaskEditorAdvanced : public QWidget
{ 
    Q_OBJECT

public:
    TaskEditorAdvanced( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TaskEditorAdvanced();

    /*
     * I could have a struct which returns a QWidget*
     * load and save to a OTodo
     * and use multiple inheretence with all other widgets
     * and then simply iterate over the list of structs
     * this way I could easily have plugins for the whole editor....
     * but I do not do it -zecke
     */
    void load( const  OTodo& );
    void save( OTodo& );

    QComboBox      *cmbState;
    QLabel         *txtMaintainer;
    QToolButton    *tbtMaintainer;
    QComboBox      *cmbMode;
    QMultiLineEdit *m_edit;

};

#endif // TASKEDITORADVANCED_H
