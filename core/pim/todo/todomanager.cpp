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
    m_db = 0l;
    QTime time;
    time.start();
    int el = time.elapsed();
    qWarning("QTimer for loading %d", el/1000 );
}
TodoManager::~TodoManager() {
    delete m_db;
}
OTodo TodoManager::event(int uid ) {
    return m_db->find( uid );
}
void TodoManager::updateList() {
    m_list = m_db->allRecords();
}
OTodoAccess::List TodoManager::list() const{
    return m_list;
}
OTodoAccess::List TodoManager::sorted( bool asc, int so, int f, int cat ) {
    return m_db->sorted( asc, so, f, cat );
}
OTodoAccess::List::Iterator TodoManager::overDue() {
    int filter = 2 | 1;
    m_list = m_db->sorted(m_asc, m_sortOrder, filter,  m_ca );
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
bool TodoManager::isLoaded()const {
    return (m_db == 0 );
}
void TodoManager::load() {
    if (!m_db) {
        m_db = new OTodoAccess();
        m_db->load();
    }
}
