/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
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

#ifndef OPIE_QUICK_EDIT_H
#define OPIE_QUICK_EDIT_H

#include <qsignal.h>
#include <qwidget.h>

#include <opie2/opimtodo.h>

using Opie::OPimTodo;

namespace Todo{
    class MainWindow;
    struct QuickEditBase {
        virtual OPimTodo todo() const = 0;
        virtual QSignal* signal() = 0;
        virtual QWidget* widget() = 0;
    };
    /*
     * this is my second try
     * of signal and slots with namespaces
     * and templates
     * I use a different approach now
     * I give a QSignal away
     * and have a protected method called emit
     */
    /**
     * Quick edit is meant to quickly enter
     * OPimTodos in a fast way
     */
    class QuickEdit : public QuickEditBase{
    public:
        QuickEdit( MainWindow* main );
        virtual ~QuickEdit();
        //OPimTodo todo() const;
        QSignal* signal();
        //QWidget* widget();
    protected:
        MainWindow* mainWindow();
        void commit();
    private:
        MainWindow* m_main;
        QSignal* m_sig;
    };
};


#endif
