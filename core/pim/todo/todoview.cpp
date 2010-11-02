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

#include "mainwindow.h"

#include <opie2/odebug.h>

#include "todoview.h"

using namespace Todo;




TodoView::TodoView( MainWindow* win )
    : m_main( win )
{
    m_asc = false;
    m_sortOrder = -1;
}

TodoView::~TodoView()
{
}

MainWindow* TodoView::todoWindow()
{
    return m_main;
}

OPimTodo TodoView::event(int uid )
{
    return m_main->event( uid );
}

OPimTodoAccess::List TodoView::list()
{
    todoWindow()->updateList();
    return todoWindow()->list();
}

OPimTodoAccess::List TodoView::sorted() const
{
    return m_sort;
}

void TodoView::sort()
{
    m_sort = todoWindow()->sorted(m_asc,m_sortOrder );
    owarn << "m_sort.count() = " << m_sort.count() << oendl;
}

void TodoView::sort(int sort)
{
    m_sort = todoWindow()->sorted(m_asc, m_sortOrder, sort );
}

void TodoView::setSortOrder( int order )
{
    m_sortOrder = order;
}

void TodoView::setAscending( bool b )
{
    owarn << "setAscending " << b << oendl;
    m_asc = b;
}

void TodoView::update(int, const SmallTodo&  )
{
    //m_main->slotUpate1( uid, to );
}

void TodoView::update(int , const OPimTodo& ev )
{
    m_main->updateTodo( ev );
}

void TodoView::showTodo( int uid )
{
    m_main->slotShow( uid );
}

void TodoView::edit( int uid )
{
    m_main->slotEdit( uid );
}

void TodoView::remove( int uid )
{
    m_main->m_todoMgr.remove( uid );
}

void TodoView::complete( int uid )
{
    m_main->slotComplete( uid );
}

void TodoView::complete( const OPimTodo& ev )
{
    m_main->slotComplete( ev );
}

void TodoView::removeQuery( int uid )
{
    m_main->slotDelete( uid );
}
