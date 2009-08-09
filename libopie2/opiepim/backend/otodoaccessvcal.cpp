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

#include <opie2/private/vobject_p.h>

/* OPIE */
#include <opie2/otodoaccessvcal.h>
#include <opie2/odebug.h>

#include <qpe/timeconversion.h>

/* QT */
//FIXME: Hack to allow direct access to FILE* fh. Rewrite this!
#define protected public
#include <qfile.h>
#undef protected

using namespace Opie;

namespace {
    static OPimTodo eventByVObj( VObject *obj ){
        OPimTodo event;
        QString itemDesc, itemSummary, itemAttachNote;
        
        event.setCompleted( false );
        
        // no uid, attendees, ... and no fun
        VObjectIterator it;
        initPropIterator( &it, obj );
        while( moreIteration( &it ) ) {
            VObject *o = nextVObject( &it );
            QCString name = vObjectName( o );
            QCString value = vObjectStringZValue( o );

            if ( name == VCDescriptionProp ) {
                itemDesc = value;
            }
            else if ( name == VCSummaryProp ) {
                itemSummary = value;
            }
            else if ( name == VCAttachProp ) {
                itemAttachNote = value;
            }
            else if ( name == VCStatusProp ) {
                if( value == "COMPLETED" )
                    event.setCompleted( true );
                else
                    event.setCompleted( false );
            }
            else if ( name == VCPriorityProp ) {
                bool ok;
                event.setPriority( value.toInt(&ok) );
            }
            else if ( name == VCDueProp ) {
                event.setHasDueDate( true );
                event.setDueDate( TimeConversion::fromISO8601( value ).date() );
            }
            else if ( name == VCCategoriesProp ) {
                // FIXME unhandled!
            }
        }
        
        // Handle description and notes
        if( itemSummary.isEmpty() ) {
            // This is typical of vcs files sent from older PocketPC & Palm
            // summary is in description field and notes are an attachment
            event.setSummary( itemDesc );
            event.setDescription( itemAttachNote );
        }
        else {
            // OTOH, KOrganiser stores events this way. Newer PocketPC versions
            // set the description the same as the summary and put notes in an
            // attachment.
            event.setSummary( itemSummary );
            if( itemAttachNote.isEmpty() )
                event.setDescription( itemDesc );
            else {
                if( itemDesc.isEmpty() || itemDesc == itemSummary )
                    event.setDescription( itemAttachNote );
                else {
                    // Just in case we have both
                    QString note = itemDesc;
                    note.append( '\n' );
                    note.append( itemAttachNote );
                    event.setDescription( note );
                }
            }
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

        addPropValue( task, VCSummaryProp,
                      event.summary().local8Bit() );

        addPropValue( task, VCDescriptionProp,
                      event.description().local8Bit() );
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

    QFile file( m_file );
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
    writeVObject( file.fh, obj ); //FIXME: HACK!!!
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

QArray<int> OPimTodoAccessVCal::effectiveToDos( const QDate& ,
                                             const QDate& ,
                                             bool  )const {
    QArray<int> ar(0);
    return ar;
}

QArray<int> OPimTodoAccessVCal::overDue()const {
    QArray<int> ar(0);
    return ar;
}

}
