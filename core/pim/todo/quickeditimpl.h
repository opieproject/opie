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

#ifndef OPIE_QUICK_EDIT_IMPL_H
#define OPIE_QUICK_EDIT_IMPL_H

#include <qpixmap.h>

#include <qtoolbar.h>

#include "quickedit.h"

class QLineEdit;
class QLabel;

class QuickEditImpl : public QToolBar, public Todo::QuickEdit {
    Q_OBJECT
public:
    QuickEditImpl( QWidget* parent,  bool visible);
    ~QuickEditImpl();
    OPimTodo todo()const;
    QWidget* widget();
private slots:
    void slotEnter();
    void slotPrio();
    void slotMore();
    void slotCancel();
private:
    void reinit();
    int m_state;
    QLabel* m_lbl;
    QLineEdit* m_edit;
    QLabel* m_enter;
    QLabel* m_more;
    QPopupMenu* m_menu;
    OPimTodo m_todo;
    bool m_visible;

    QPixmap priority1;
    QPixmap priority3;
    QPixmap priority5;
};

#endif
