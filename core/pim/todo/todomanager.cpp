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
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
#include <qpe/categoryselect.h>

#include "todomanager.h"

using namespace Todo;

TodoManager::TodoManager( QObject *obj )
    : QObject( obj ) {
    m_db = new Opie::ToDoDB();
}
TodoManager::~TodoManager() {
    delete m_db;
}
Opie::ToDoEvent TodoManager::event(int uid ) {
    return m_db->findEvent( uid );
}
Opie::ToDoDB::Iterator TodoManager::begin() {
    return m_db->rawToDos();
}
Opie::ToDoDB::Iterator TodoManager::end() {
    return m_db->end();
}
Opie::ToDoDB::Iterator TodoManager::overDue() {
    return m_db->overDue();
}
Opie::ToDoDB::Iterator TodoManager::fromTo( const QDate& start,
                                      const QDate& end ) {
    return m_db->effectiveToDos( start, end );
}
Opie::ToDoDB::Iterator TodoManager::query( const ToDoEvent& ev, int query ) {
    return m_db->queryByExample( ev, query );
}
Opie::ToDoDB* TodoManager::todoDB() {
    return m_db;
}
void TodoManager::add( const Opie::ToDoEvent& ev ) {
    m_db->addEvent( ev );
}
void TodoManager::update( int, const SmallTodo& ) {

}
void TodoManager::update( int, const Opie::ToDoEvent& ev) {
    m_db->replaceEvent( ev );
}
void TodoManager::remove( int uid ) {
    Opie::ToDoEvent ev = m_db->findEvent( uid );
    m_db->removeEvent( ev );
}
void TodoManager::removeAll() {
    m_db->removeAll();
}
void TodoManager::save() {
    m_db->save();
}
bool TodoManager::saveAll() {
    return m_db->save();
}
void TodoManager::reload() {
    m_db->reload();
}
QStringList TodoManager::categories() {
    m_cat.load(categoryFileName() );
    return m_cat.labels( "Todo List");
}
/*
 * we rely on load beeing called from populateCategories
 */
int TodoManager::catId( const QString& cats ) {
    return m_cat.id( "Todo List", cats );
}
