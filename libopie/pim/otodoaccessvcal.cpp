#include <qfile.h>

#include <qtopia/private/vobject_p.h>
#include <qtopia/timeconversion.h>
#include <qtopia/private/qfiledirect_p.h>

#include "otodoaccessvcal.h"

namespace {
    static OTodo eventByVObj( VObject *obj ){
        OTodo event;
        VObject *ob;
        QCString name;
        // no uid, attendees, ... and no fun
        // description
        if( ( ob = isAPropertyOf( obj, VCDescriptionProp )) != 0 ){
            name = vObjectStringZValue( ob );
            event.setDescription( name );
        }
        // summary
        if ( ( ob = isAPropertyOf( obj,  VCSummaryProp ) ) != 0 ) {
            name = vObjectStringZValue( ob );
            event.setSummary( name );
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
    static VObject *vobjByEvent( const OTodo &event )  {
        VObject *task = newVObject( VCTodoProp );
        if( task == 0 )
            return 0l;

        if( event.hasDueDate() )
            addPropValue( task, VCDueProp,
                          TimeConversion::toISO8601( event.dueDate() ) );

        if( event.isCompleted() )
            addPropValue( task, VCStatusProp, "COMPLETED");

        QString string = QString::number(event.priority() );
        addPropValue( task, VCPriorityProp, string.local8Bit() );

        addPropValue( task, VCCategoriesProp,
                      event.idsToString( event.categories() ).local8Bit() );

        addPropValue( task, VCDescriptionProp,
                      event.description().local8Bit() );

        addPropValue( task, VCSummaryProp,
                      event.summary().local8Bit() );
  return task;
};
}

OTodoAccessVCal::OTodoAccessVCal( const QString& path )
    : m_dirty(false), m_file( path )
{
}
OTodoAccessVCal::~OTodoAccessVCal() {
}
bool OTodoAccessVCal::load() {
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
            OTodo to = eventByVObj( vobj );
            m_map.insert( to.uid(), to );
        }
    }

    // Should I do a delete vcal?

    return true;
}
bool OTodoAccessVCal::reload() {
    return load();
}
bool OTodoAccessVCal::save() {
    if (!m_dirty )
        return true;

    QFileDirect file( m_file );
    if (!file.open(IO_WriteOnly ) )
        return false;

    VObject *obj;
    obj = newVObject( VCCalProp );
    addPropValue( obj, VCVersionProp, "1.0" );
    VObject *vo;
    for(QMap<int, OTodo>::ConstIterator it=m_map.begin(); it !=m_map.end(); ++it ){
        vo = vobjByEvent( it.data() );
        addVObjectProp(obj, vo );
    }
    writeVObject( file.directHandle(), obj );
    cleanVObject( obj );
    cleanStrTbl();

    m_dirty = false;
    return true;
}
void OTodoAccessVCal::clear() {
    m_map.clear();
    m_dirty = true;
}
bool OTodoAccessVCal::add( const OTodo& to ) {
    m_map.insert( to.uid(), to );
    m_dirty = true;
    return true;
}
bool OTodoAccessVCal::remove( int uid ) {
    m_map.remove( uid );
    m_dirty = true;
    return true;
}
bool OTodoAccessVCal::replace( const OTodo& to ) {
    m_map.replace( to.uid(), to );
    m_dirty = true;
    return true;
}
OTodo OTodoAccessVCal::find(int uid )const {
    return m_map[uid];
}
QArray<int> OTodoAccessVCal::sorted( bool, int, int, int ) {
    QArray<int> ar(0);
    return ar;
}
QArray<int> OTodoAccessVCal::allRecords()const {
    QArray<int> ar( m_map.count() );
    QMap<int, OTodo>::ConstIterator it;
    int i = 0;
    for ( it = m_map.begin(); it != m_map.end(); ++it ) {
        ar[i] = it.key();
        i++;
    }
    return ar;
}
QArray<int> OTodoAccessVCal::queryByExample( const OTodo&, int ) {
    QArray<int> ar(0);
    return ar;
}
QArray<int> OTodoAccessVCal::effectiveToDos( const QDate& ,
                                             const QDate& ,
                                             bool  ) {
    QArray<int> ar(0);
    return ar;
}
QArray<int> OTodoAccessVCal::overDue() {
    QArray<int> ar(0);
    return ar;
}
