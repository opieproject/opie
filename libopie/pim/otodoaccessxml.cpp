#include <qfile.h>

#include <qpe/global.h>
#include <qpe/stringutil.h>
#include <qpe/timeconversion.h>

#include <opie/xmltree.h>

#include "otodoaccessxml.h"

OTodoAccessXML::OTodoAccessXML( const QString& appName,
                                const QString& fileName )
    : OTodoAccessBackend(), m_app( appName ),  m_opened( false ), m_changed( false )
{
    if (!fileName.isEmpty() )
        m_file = fileName;
    else
        m_file = Global::applicationFileName( "todolist", "todolist.xml" );
}
OTodoAccessXML::~OTodoAccessXML() {

}
bool OTodoAccessXML::load() {
    m_opened = true;
    m_changed = false;
    /* initialize dict */
    /*
     * UPDATE dict if you change anything!!!
     */
    QAsciiDict<int> dict(15);
    dict.setAutoDelete( TRUE );
    dict.insert("Categories" ,     new int(OTodo::Category)         );
    dict.insert("Uid" ,            new int(OTodo::Uid)              );
    dict.insert("HasDate" ,        new int(OTodo::HasDate)          );
    dict.insert("Completed" ,      new int(OTodo::Completed)        );
    dict.insert("Description" ,    new int(OTodo::Description)      );
    dict.insert("Summary" ,        new int(OTodo::Summary)          );
    dict.insert("Priority" ,       new int(OTodo::Priority)         );
    dict.insert("DateDay" ,        new int(OTodo::DateDay)          );
    dict.insert("DateMonth" ,      new int(OTodo::DateMonth)        );
    dict.insert("DateYear" ,       new int(OTodo::DateYear)         );
    dict.insert("Progress" ,       new int(OTodo::Progress)         );
    dict.insert("Completed",       new int(OTodo::Completed)        );
    dict.insert("CrossReference",  new int(OTodo::CrossReference)   );
    dict.insert("HasAlarmDateTime",new int(OTodo::HasAlarmDateTime) );
    dict.insert("AlarmDateTime",   new int(OTodo::AlarmDateTime)    );

    Opie::XMLElement *root = Opie::XMLElement::load( m_file );
    int day, year, month;
    day = year = month = -1;

    /* if opened */
    if ( root != 0l ) {
        Opie::XMLElement *element = root->firstChild();
        if ( element == 0l )
            return false;

        element = element->firstChild();

        while ( element ) {
            if ( element->tagName() != QString::fromLatin1("Task") ) {
                element = element->nextChild();
                continue;
            }
            /* here is the right element for a task */
            OTodo ev = todo( &dict, element );
            m_events.insert( ev.uid(), ev );

            element = element->nextChild();
        }
    }else {
//        qWarning("could not parse");
        return false;;
    }
    delete root;

//    qWarning("Access %d" + m_events.count() );
    return true;
}
bool OTodoAccessXML::reload() {
    return load();
}
bool OTodoAccessXML::save() {
//    qWarning("saving");
    if (!m_opened || !m_changed ) {
//        qWarning("not saving");
        return true;
    }
    QString strNewFile = m_file + ".new";
    QFile f( strNewFile );
    if (!f.open( IO_WriteOnly|IO_Raw ) )
        return false;

    int written;
    QString out;
    out = "<!DOCTYPE Tasks>\n<Tasks>\n";

    // for all todos
    QMap<int, OTodo>::Iterator it;
    for (it = m_events.begin(); it != m_events.end(); ++it ) {
        out+= "<Task " + toString( (*it) ) + " />\n";
        QCString cstr = out.utf8();
        written = f.writeBlock( cstr.data(),  cstr.length() );

        /* less written then we wanted */
        if ( written != (int)cstr.length() ) {
            f.close();
            QFile::remove( strNewFile );
            return false;
        }
        out = QString::null;
    }

    out +=  "</Tasks>";
    QCString cstr = out.utf8();
    written = f.writeBlock( cstr.data(), cstr.length() );

    if ( written != (int)cstr.length() ) {
        f.close();
        QFile::remove( strNewFile );
        return false;
    }
    /* flush before renaming */
    f.close();

    if( ::rename( strNewFile.latin1(),  m_file.latin1() ) < 0 ) {
//        qWarning("error renaming");
        QFile::remove( strNewFile );
    }

    m_changed = false;
    return true;
}
QArray<int> OTodoAccessXML::allRecords()const {
    QArray<int> ids( m_events.count() );
    QMap<int, OTodo>::ConstIterator it;
    int i = 0;

    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        ids[i] = it.key();
        i++;
    }
    return ids;
}
QArray<int> OTodoAccessXML::queryByExample( const OTodo&, int sort ) {
    QArray<int> ids(0);
    return ids;
}
OTodo OTodoAccessXML::find( int uid )const {
    OTodo todo;
    todo.setUid( 0 ); // isEmpty()
    QMap<int, OTodo>::ConstIterator it = m_events.find( uid );
    if ( it != m_events.end() )
        todo = it.data();

    return todo;
}
void OTodoAccessXML::clear() {
    if (m_opened )
        m_changed = true;

    m_events.clear();
}
bool OTodoAccessXML::add( const OTodo& todo ) {
//    qWarning("add");
    m_changed = true;
    m_events.insert( todo.uid(), todo );

    return true;
}
bool OTodoAccessXML::remove( int uid ) {
    m_changed = true;
    m_events.remove( uid );

    return true;
}
bool OTodoAccessXML::replace( const OTodo& todo) {
    m_changed = true;
    m_events.replace( todo.uid(), todo );

    return true;
}
QArray<int> OTodoAccessXML::effectiveToDos( const QDate& start,
                                            const QDate& end,
                                            bool includeNoDates ) {
    QArray<int> ids( m_events.count() );
    QMap<int, OTodo>::Iterator it;

    int i = 0;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( !it.data().hasDueDate() ) {
            if ( includeNoDates ) {
                ids[i] = it.key();
                i++;
            }
        }else if ( it.data().dueDate() >= start &&
                   it.data().dueDate() <= end ) {
            ids[i] = it.key();
            i++;
        }
    }
    ids.resize( i );
    return ids;
}
QArray<int> OTodoAccessXML::overDue() {
    QArray<int> ids( m_events.count() );
    int i = 0;

    QMap<int, OTodo>::Iterator it;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().isOverdue() ) {
            ids[i] = it.key();
            i++;
        }
    }
    ids.resize( i );
    return ids;
}


/* private */
OTodo OTodoAccessXML::todo( QAsciiDict<int>* dict, Opie::XMLElement* element)const {
//    qWarning("parse to do from XMLElement" );
    OTodo ev;
    QMap<QString, QString> attributes = element->attributes();
    QMap<QString, QString>::Iterator it;

    int *find=0;
    int day, month, year;
    day = month = year = -1;
    for ( it = attributes.begin(); it != attributes.end(); ++it ) {
        find = (*dict)[ it.key() ];
        if (!find ) {
//            qWarning("Unknown option" + it.key() );
            ev.setCustomField( it.key(), it.data() );
            continue;
        }

        switch( *find ) {
        case OTodo::Uid:
            ev.setUid( it.data().toInt() );
            break;
        case OTodo::Category:
            ev.setCategories( ev.idsFromString( it.data() ) );
            break;
        case OTodo::HasDate:
            ev.setHasDueDate( it.data().toInt() );
            break;
        case OTodo::Completed:
            ev.setCompleted( it.data().toInt() );
            break;
        case OTodo::Description:
            ev.setDescription( it.data() );
            break;
        case OTodo::Summary:
            ev.setSummary( it.data() );
            break;
        case OTodo::Priority:
            ev.setPriority( it.data().toInt() );
            break;
        case OTodo::DateDay:
            day = it.data().toInt();
            break;
        case OTodo::DateMonth:
            month = it.data().toInt();
            break;
        case OTodo::DateYear:
            year = it.data().toInt();
            break;
        case OTodo::Progress:
            ev.setProgress( it.data().toInt() );
            break;
        case OTodo::CrossReference:
        {
         /*
          * A cross refernce looks like
          * appname,id;appname,id
          * we need to split it up
          */
            QStringList refs = QStringList::split(';', it.data() );
            QStringList::Iterator strIt;
            for (strIt = refs.begin(); strIt != refs.end(); ++strIt ) {
                int pos = (*strIt).find(',');
                if ( pos > -1 )
                    ev.addRelation( (*strIt).left(pos),  (*strIt).mid(pos+1).toInt() );

            }
            break;
        }
        case OTodo::HasAlarmDateTime:
            ev.setHasAlarmDateTime( it.data().toInt() );
            break;
        case OTodo::AlarmDateTime: {
            /* this sounds better ;) zecke */
            ev.setAlarmDateTime( TimeConversion::fromISO8601( it.data().local8Bit() ) );
            break;
        }
        default:
            break;
        }
    }
    if ( ev.hasDueDate() ) {
        QDate date( year,  month, day );
        ev.setDueDate( date );
    }

    return ev;
}
QString OTodoAccessXML::toString( const OTodo& ev )const {
    QString str;

    str += "Completed=\"" + QString::number( ev.isCompleted() ) + "\" ";
    str += "HasDate=\"" + QString::number( ev.hasDueDate() ) + "\" ";
    str += "Priority=\"" + QString::number( ev.priority() ) + "\" ";
    str += "Progress=\"" + QString::number(ev.progress() ) + "\" ";

    str += "Categories=\"" + toString( ev.categories() ) + "\" ";
    str += "Description=\"" + Qtopia::escapeString( ev.description() ) + "\" ";
    str += "Summary=\"" + Qtopia::escapeString( ev.summary() ) + "\" ";

    if ( ev.hasDueDate() ) {
        str += "DateYear=\"" + QString::number( ev.dueDate().year() ) + "\" ";
        str += "DateMonth=\"" + QString::number( ev.dueDate().month() ) + "\" ";
        str += "DateDay=\"" + QString::number( ev.dueDate().day() ) + "\" ";
    }
//    qWarning( "Uid %d",  ev.uid() );
    str += "Uid=\"" + QString::number( ev.uid() ) + "\" ";

// append the extra options
    /* FIXME Qtopia::Record this is currently not
     * possible you can set custom fields
     * but don' iterate over the list
     * I may do #define private protected
     * for this case - cough  --zecke
     */
    /*
    QMap<QString, QString> extras = ev.extras();
    QMap<QString, QString>::Iterator extIt;
    for (extIt = extras.begin(); extIt != extras.end(); ++extIt )
        str += extIt.key() + "=\"" +  extIt.data() + "\" ";
    */
    // cross refernce
    QStringList list = ev.relatedApps();
    QStringList::Iterator listIt;
    QString refs;
    str += "CrossReference=\"";
    bool added = false;
    for ( listIt = list.begin(); listIt != list.end(); ++listIt ) {
        added = true;
        QArray<int> ints = ev.relations( (*listIt) );
        for ( uint i = 0; i< ints.count(); i++ ) {
            str += (*listIt) + "," + QString::number( i ) + ";";
        }
    }
    if ( added )
        str = str.remove( str.length()-1, 1 );

    str += "\" ";

    str += "AlarmDateTime=\"" + TimeConversion::toISO8601( ev.alarmDateTime() ) + "\" ";

    return str;
}
QString OTodoAccessXML::toString( const QArray<int>& ints ) const {
    return Qtopia::Record::idsToString( ints );
}
