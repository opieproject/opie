/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
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
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OPIE_TASK_EDITOR_H
#define OPIE_TASK_EDITOR_H

#include <qdialog.h>

#include <opie2/opimtodo.h>
#include <opie2/otabwidget.h>
#include <opie2/opimrecurrencewidget.h>
#include <opie2/opimrecurrencewidget.h>

class TaskEditorOverView;
class TaskEditorStatus;
class TaskEditorAlarms;
class QMultiLineEdit;

class OTaskEditor : public QDialog {
    Q_OBJECT
public:
    OTaskEditor(int cur);
    OTaskEditor( const Opie::OPimTodo& todo );
    ~OTaskEditor();

    /*
     * same as the c'tor but this gives us the
     * power to 'preload' the dialog
     */
    void init( int cur );
    void init( const Opie::OPimTodo& todo );

    Opie::OPimTodo todo()const;
protected:
    void showEvent( QShowEvent* );
private:
    void load( const Opie::OPimTodo& );
    void init();

    Opie::Ui::OTabWidget         *m_tab;
    TaskEditorOverView *m_overView;
    TaskEditorStatus   *m_stat;
    TaskEditorAlarms   *m_alarm;
    TaskEditorAlarms   *m_remind;
    OPimRecurrenceWidget  *m_rec;
    Opie::OPimTodo		m_todo;
};

#endif
