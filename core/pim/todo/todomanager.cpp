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
#include <qdatetime.h>
#include <qpe/categoryselect.h>

#include "todomanager.h"

using namespace Todo;

TodoManager::TodoManager( QObject *obj )
    : QObject( obj ) {
    m_db = new OTodoAccess();
    QTime time;
    time.start();
    m_db->load();
    int el = time.elapsed();
    qWarning("QTimer for loading %d", el/1000 );
}
TodoManager::~TodoManager() {
    delete m_db;
}
OTodo TodoManager::event(int uid ) {
    return m_db->find( uid );
}
OTodoAccess::List::Iterator TodoManager::begin() {
    m_list = m_db->allRecords();
    m_it = m_list.begin();
    return m_it;
}
OTodoAccess::List::Iterator TodoManager::end() {
    return m_list.end();
}
OTodoAccess::List::Iterator TodoManager::overDue() {
    m_list = m_db->overDue();
    m_it = m_list.begin();
    return m_it;
}
OTodoAccess::List::Iterator TodoManager::fromTo( const QDate& start,
                                      const QDate& end ) {
    m_list = m_db->effectiveToDos( start, end );
    m_it = m_list.begin();
    return m_it;
}
OTodoAccess::List::Iterator TodoManager::query( const OTodo& ev, int query ) {
    m_list = m_db->queryByExample( ev, query );
    m_it = m_list.begin();
    return m_it;
}
OTodoAccess* TodoManager::todoDB() {
    return m_db;
}
void TodoManager::add( const OTodo& ev ) {
    m_db->add( ev );
}
void TodoManager::update( int, const SmallTodo& ) {

}
void TodoManager::update( int, const OTodo& ev) {
    m_db->replace( ev );
}
void TodoManager::remove( int uid ) {
    m_db->remove( uid );
}
void TodoManager::removeAll() {
    m_db->clear();
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
void TodoManager::remove( const QArray<int>& ids) {
    for (uint i=0; i < ids.size(); i++ )
        remove( ids[i] );
}
