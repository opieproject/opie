/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#include <qfile.h>

#include <qtopia/private/vobject_p.h>
#include <qtopia/timeconversion.h>
#include <qtopia/private/qfiledirect_p.h>

#include <opie2/otodoaccessvcal.h>

using namespace Opie;

namespace {
    static OPimTodo eventByVObj( VObject *obj ){
        OPimTodo event;
        VObject *ob;
        QCString name;
        // no uid, attendees, ... and no fun
        // description
        if( ( ob = isAPropertyOf( obj, VCDescriptionProp )) != 0 ){
            name = vObjectStringZValue( ob );
#if 0
            event.setDescription( name );
#else
            event.setSummary( name );
#endif
        }
        // summary
        if ( ( ob = isAPropertyOf( obj,  VCSummaryProp ) ) != 0 ) {
            name = vObjectStringZValue( ob );
#if 0
            event.setSummary( name );
#else
            event.setDescription( name );
#endif
        }
        // completed
        if( ( ob = isAPropertyOf( obj, VCStatusProp )) != 0 ){
            name = vObjectStringZValue( ob );
            if( name == "COMPLETED" ){
                event.setCompleted( true );
            }else{
                event.setCompleted( false );
            }
        }else
            event.setCompleted( false );
        // priority
        if ((ob = isAPropertyOf(obj, VCPriorityProp))) {
            name = vObjectStringZValue( ob );
            bool ok;
            event.setPriority(name.toInt(&ok) );
        }
        //due date
        if((ob = isAPropertyOf(obj, VCDueProp)) ){
            event.setHasDueDate( true );
            name = vObjectStringZValue( ob );
            event.setDueDate( TimeConversion::fromISO8601( name).date() );
        }
        // categories
        if((ob = isAPropertyOf( obj, VCCategoriesProp )) != 0 ){
            name = vObjectStringZValue( ob );
            qWarning("Categories:%s", name.data() );
        }

        event.setUid( 1 );
        return event;
    };
    static VObject *vobjByEvent( const OPimTodo &event )  {
        VObject *task = newVObject( VCTodoProp );
        if( task == 0 )
            return 0l;

        if( event.hasDueDate() ) {
            QTime time(0, 0, 0);
            QDateTime date(event.dueDate(), time );
            addPropValue( task, VCDueProp,
                          TimeConversion::toISO8601( date ) );
        }

        if( event.isCompleted() )
            addPropValue( task, VCStatusProp, "COMPLETED");

        QString string = QString::number(event.priority() );
        addPropValue( task, VCPriorityProp, string.local8Bit() );

        addPropValue( task, VCCategoriesProp,
                      event.idsToString( event.categories() ).local8Bit() );

#if 0

	// There seems a misrepresentation between summary in otodoevent
	// and summary in vcard. 
	// The same with description..
	// Description is summary and vice versa.. Argh.. (eilers)


        addPropValue( task, VCDescriptionProp,
                      event.description().local8Bit() );

        addPropValue( task, VCSummaryProp,
                      event.summary().local8Bit() );

#else
        addPropValue( task, VCDescriptionProp,
                      event.summary().local8Bit() );

        addPropValue( task, VCSummaryProp,
                      event.description().local8Bit() );
#endif 
  return task;
};
}

namespace Opie {
OPimTodoAccessVCal::OPimTodoAccessVCal( const QString& path )
    : m_dirty(false), m_file( path )
{
}
OPimTodoAccessVCal::~OPimTodoAccessVCal() {
}
bool OPimTodoAccessVCal::load() {
    m_map.clear();
    m_dirty = false;

    VObject* vcal = 0l;
    vcal = Parse_MIME_FromFileName( QFile::encodeName(m_file).data() );
    if (!vcal )
        return false;

    // Iterate over the list
    VObjectIterator it;
    VObject* vobj;

    initPropIterator(&it, vcal);

    while( moreIteration( &it ) ) {
        vobj = ::nextVObject( &it );
        QCString name = ::vObjectName( vobj );
        if( name == VCTodoProp ){
            OPimTodo to = eventByVObj( vobj );
            m_map.insert( to.uid(), to );
        }
    }

    // Should I do a delete vcal?

    return true;
}
bool OPimTodoAccessVCal::reload() {
    return load();
}
bool OPimTodoAccessVCal::save() {
    if (!m_dirty )
        return true;

    QFileDirect file( m_file );
    if (!file.open(IO_WriteOnly ) )
        return false;

    VObject *obj;
    obj = newVObject( VCCalProp );
    addPropValue( obj, VCVersionProp, "1.0" );
    VObject *vo;
    for(QMap<int, OPimTodo>::ConstIterator it=m_map.begin(); it !=m_map.end(); ++it ){
        vo = vobjByEvent( it.data() );
        addVObjectProp(obj, vo );
    }
    writeVObject( file.directHandle(), obj );
    cleanVObject( obj );
    cleanStrTbl();

    m_dirty = false;
    return true;
}
void OPimTodoAccessVCal::clear() {
    m_map.clear();
    m_dirty = true;
}
bool OPimTodoAccessVCal::add( const OPimTodo& to ) {
    m_map.insert( to.uid(), to );
    m_dirty = true;
    return true;
}
bool OPimTodoAccessVCal::remove( int uid ) {
    m_map.remove( uid );
    m_dirty = true;
    return true;
}
void OPimTodoAccessVCal::removeAllCompleted() {
    for ( QMap<int, OPimTodo>::Iterator it = m_map.begin(); it != m_map.end(); ++it ) {
        if ( (*it).isCompleted() )
            m_map.remove( it );
    }
}
bool OPimTodoAccessVCal::replace( const OPimTodo& to ) {
    m_map.replace( to.uid(), to );
    m_dirty = true;
    return true;
}
OPimTodo OPimTodoAccessVCal::find(int uid )const {
    return m_map[uid];
}
QArray<int> OPimTodoAccessVCal::sorted( bool, int, int, int ) {
    QArray<int> ar(0);
    return ar;
}
QArray<int> OPimTodoAccessVCal::allRecords()const {
    QArray<int> ar( m_map.count() );
    QMap<int, OPimTodo>::ConstIterator it;
    int i = 0;
    for ( it = m_map.begin(); it != m_map.end(); ++it ) {
        ar[i] = it.key();
        i++;
    }
    return ar;
}
QArray<int> OPimTodoAccessVCal::matchRegexp(const QRegExp& /* r */)const {
    QArray<int> ar(0);
    return ar;
}
QArray<int> OPimTodoAccessVCal::queryByExample( const OPimTodo&, int, const QDateTime& ) {
    QArray<int> ar(0);
    return ar;
}
QArray<int> OPimTodoAccessVCal::effectiveToDos( const QDate& ,
                                             const QDate& ,
                                             bool  ) {
    QArray<int> ar(0);
    return ar;
}
QArray<int> OPimTodoAccessVCal::overDue() {
    QArray<int> ar(0);
    return ar;
}
QBitArray OPimTodoAccessVCal::supports()const {
    static QBitArray ar = sup();

    return ar;
}
QBitArray OPimTodoAccessVCal::sup() {
    QBitArray ar ( OPimTodo::CompletedDate +1 );
    ar.fill( true );

    ar[OPimTodo::CrossReference] = false;
    ar[OPimTodo::State ] = false;
    ar[OPimTodo::Reminders] = false;
    ar[OPimTodo::Notifiers] = false;
    ar[OPimTodo::Maintainer] = false;
    ar[OPimTodo::Progress] = false;
    ar[OPimTodo::Alarms ] = false;
    ar[OPimTodo::Recurrence] = false;

    return ar;
}

}
