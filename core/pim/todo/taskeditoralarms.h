/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002,2003 <>
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

#ifndef TASKEDITORALARMS_H
#define TASKEDITORALARMS_H

#include <opie2/opimtodo.h>

#include <qwidget.h>

class QListView;
class QListViewItem;

using namespace Opie;

class AlarmItem;
class DateBookMonth;
class TaskEditorAlarms : public QWidget
{
    Q_OBJECT

public:
    enum Type {
        Alarm = 0,
        Reminders
    };
    TaskEditorAlarms( QWidget* parent = 0, int type = Alarm,  const char* name = 0, WFlags fl = 0 );
    ~TaskEditorAlarms();
    void load( const OPimTodo& );
    void save( OPimTodo& );
private:
    QPopupMenu* popup( int column );
    void inlineSetDate( AlarmItem*, const QPoint& p );
    void inlineSetTime( AlarmItem*);
    void inlineSetType( AlarmItem*, const QPoint& p );

    QListView* lstAlarms;
    QPopupMenu* m_date;
    QPopupMenu* m_time;
    QPopupMenu* m_type;
    DateBookMonth* m_dbMonth;

protected slots:
    void slotNew();
    void slotEdit();
    void slotDelete();
    void inlineEdit( QListViewItem*, const QPoint& p, int );
};

#endif // TASKEDITORALARMS_H
