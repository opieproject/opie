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

#include <opie2/odebug.h>
#include <opie2/otodoaccess.h>
#include <opie2/otodoaccessxml.h>

#include <qpe/config.h>
#include <qpe/global.h>

#include "todotemplatemanager.h"


using namespace Todo;

TemplateManager::TemplateManager() {
    m_path =  Global::applicationFileName("todolist", "templates.xml");
}
TemplateManager::~TemplateManager() {
    save();
}
void TemplateManager::load() {
    Config conf("todolist_templates");
    OPimTodoAccessXML *xml = new OPimTodoAccessXML( QString::fromLatin1("template"),
                                                m_path );
    OPimTodoAccess todoDB(xml );
    todoDB.load();

    OPimTodoAccess::List::Iterator it;
    OPimTodoAccess::List list = todoDB.allRecords();
    for ( it = list.begin(); it != list.end(); ++it ) {
        OPimTodo ev = (*it);
        conf.setGroup( QString::number( ev.uid() ) );
        QString str = conf.readEntry("Name", QString::null );
        if (str.isEmpty() )
            continue;

        m_templates.insert( str, ev );
    }
}
void TemplateManager::save() {
    Config conf("todolist_templates");

    OPimTodoAccessXML *res = new OPimTodoAccessXML( "template",
                                                m_path );
    OPimTodoAccess db(res);
    db.load();
    db.clear();


    QMap<QString, OPimTodo>::Iterator it;
    for ( it = m_templates.begin(); it != m_templates.end(); ++it ) {
        OPimTodo ev = it.data();
        conf.setGroup( QString::number( ev.uid() ) );
        conf.writeEntry("Name", it.key() );
        db.add( ev );
    }
    db.save();
}
void TemplateManager::addEvent( const QString& str,
                                const OPimTodo& ev) {
    OPimTodo todo = ev;
    if( ev.uid() == 0 )
	todo.setUid(1); // generate a new uid

    m_templates.replace( str,  todo );
}
void TemplateManager::removeEvent( const QString& str ) {
    m_templates.remove( str );
}
QStringList TemplateManager::templates() const {
    QStringList list;
    QMap<QString, OPimTodo>::ConstIterator it;
    for (it = m_templates.begin(); it != m_templates.end(); ++it ) {
        list << it.key();
    }

    return list;
}
OPimTodo TemplateManager::templateEvent( const QString& templateName ) {
    return m_templates[templateName];
}
