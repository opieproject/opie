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

#ifndef TASKEDITORSTATUS_H
#define TASKEDITORSTATUS_H

#include <qdatetime.h>
#include <qwidget.h>

class DateBookMonth;
class OTodo;
class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QToolButton;

/**
 * This is the implementation of the Opie Task Editor Advanced tab
 * it features the State!
 *                 MaintainerMode
 *                 Description
 */
class TaskEditorStatus : public QWidget
{ 
    Q_OBJECT

public:
    TaskEditorStatus( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TaskEditorStatus();

    QComboBox      *cmbStatus;
    QLabel         *txtMaintainer;
    QToolButton    *tbtMaintainer;
    QComboBox      *cmbMaintMode;
    QCheckBox      *ckbDue;
    QPushButton    *btnDue;
    QCheckBox      *ckbStart;
    QPushButton    *btnStart;
    QCheckBox      *ckbComp;
    QPushButton    *btnComp;
    QComboBox      *cmbProgress;

    /*
     * I could have a struct which returns a QWidget*
     * load and save to a OTodo
     * and use multiple inheretence with all other widgets
     * and then simply iterate over the list of structs
     * this way I could easily have plugins for the whole editor....
     * but I do not do it -zecke
     */
    void load( const OTodo & );
    void save( OTodo & );

private:
    QDate          m_start;
    QDate          m_comp;
    QDate          m_due;
    DateBookMonth *m_startBook;
    DateBookMonth *m_compBook;
    DateBookMonth *m_dueBook;

private slots:
    void slotStartChecked();
    void slotCompChecked();
    void slotDueChecked();
    void slotStartChanged( int, int, int );
    void slotCompChanged( int, int, int );
    void slotDueChanged( int, int, int );

signals:
    void dueDateChanged( const QDate& date );
};

#endif // TASKEDITORSTATUS_H
