#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <qasciidict.h>
#include <qfile.h>
#include <qmap.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <qpe/global.h>
#include <qpe/stringutil.h>
#include <qpe/timeconversion.h>

#include <opie/xmltree.h>

#include "todoxmlresource.h"


using namespace Opie;

ToDoXMLResource::ToDoXMLResource(const QString& appname,
                                 const QString& fileName = QString::null )
    : ToDoResource( appname )
{
    if (!fileName.isEmpty() )
        m_file = fileName;
    else
        m_file = Global::applicationFileName("todolist", "todolist.xml" );
}
ToDoXMLResource::~ToDoXMLResource() {

}
bool ToDoXMLResource::load() {
    /* initialize dict */
    QAsciiDict<int> dict(12);
    dict.setAutoDelete( TRUE );
    dict.insert("Categories" , new int(ToDoEvent::Category) );
    dict.insert("Uid" , new int(ToDoEvent::Uid) );
    dict.insert("HasDate" , new int(ToDoEvent::HasDate) );
    dict.insert("Completed" , new int(ToDoEvent::Completed) );
    dict.insert("Description" , new int(ToDoEvent::Description) );
    dict.insert("Summary" , new int(ToDoEvent::Summary) );
    dict.insert("Priority" , new int(ToDoEvent::Priority) );
    dict.insert("DateDay" , new int(ToDoEvent::DateDay) );
    dict.insert("DateMonth" , new int(ToDoEvent::DateMonth) );
    dict.insert("DateYear" , new int(ToDoEvent::DateYear) );
    dict.insert("Progress" , new int(ToDoEvent::Progress) );
    dict.insert("Completed",  new int(ToDoEvent::Completed) );
    dict.insert("CrossReference", new int(ToDoEvent::CrossReference) );
    dict.insert("HasAlarmDateTime", new int(ToDoEvent::HasAlarmDateTime) );
    dict.insert("AlarmDateTime", new int(ToDoEvent::AlarmDateTime) );

    qWarning("loading tododb" );

    XMLElement *root = XMLElement::load( m_file );
    int day, year, month;
    day = year = month = -1;

    /* if opened */
    if ( root != 0l ) {
        qWarning("ToDoDB::load tagName():" + root->tagName() );
        XMLElement *element = root->firstChild();
        if ( element == 0l )
            return false;

        element = element->firstChild();
        while ( element ) {
            if ( element->tagName() != QString::fromLatin1("Tasks") ) {
                element = element->nextChild();
                continue;
            }
            /* here is the right element for a task */
            ToDoEvent ev = todo( &dict, element );
            m_events.insert( ev.uid(), ev );

            element = element->nextChild();
        }
        return true;
    }else {
        qWarning("could not parse");
        return false;;
    }
    delete root;

    return true;
}
bool ToDoXMLResource::save() {
    QString strNewFile = m_file + ".new";
    QFile f( strNewFile );
    if (!f.open( IO_WriteOnly|IO_Raw ) )
        return false;

    int written;
    QString out;
    out = "<!DOCTYPE Tasks>\n<Tasks>\n";

    // for all todos
    QMap<int, ToDoEvent>::Iterator it;
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
        qWarning("error renaming");
        QFile::remove( strNewFile );
    }

    return true;
}
bool ToDoXMLResource::reload() {
    return load();
}
// FIXME use stat + mtime in load
bool ToDoXMLResource::wasChangedExternally()const {
    return false;
}
ToDoEvent ToDoXMLResource::findEvent( int uid,
                                      bool *ok ) {
    ToDoEvent ev;

    /* initialize ok */
    if (ok ) *ok =false;

    QMap<int, ToDoEvent>::Iterator it = m_events.find( uid );
    if ( it != m_events.end() ) {
        ev = it.data();
        if ( ok ) *ok = true;
    }
    return ev;
}
QArray<int> ToDoXMLResource::rawToDos() {
    QArray<int> ids( m_events.count() );
    QMap<int, ToDoEvent>::Iterator it;
    int i = 0;

    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        ids[i] =  it.key();
        i++;
    }

    return ids;
}
QArray<int> ToDoXMLResource::effectiveToDos( const QDate& date,
                                             const QDate& end,
                                             bool includeNotes ) {
    QArray<int> ids( m_events.count() );
    QMap<int, ToDoEvent>::Iterator it;

    int i = 0;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().hasDueDate() ) {
            if ( includeNotes ) {
                ids[i] = it.key();
                i++;
            }
        }else if ( it.data().dueDate() >= date &&
                   it.data().dueDate() <= end ) {
            ids[i] = it.key();
            i++;
        }
    }
    ids.resize( i );

    return ids;
}
QArray<int> ToDoXMLResource::overDue() {
    QArray<int> ids( m_events.count() );
    int i = 0;

    QMap<int, ToDoEvent>::Iterator it;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().isOverdue() ) {
            ids[i] = it.key();
            i++;
        }
    }

    ids.resize( i );
    return ids;
}
QArray<int> ToDoXMLResource::queryByExample( const ToDoEvent&,
                                             uint querysettings ) {

}
bool ToDoXMLResource::addEvent( const ToDoEvent& ev ){
    m_events.insert( ev.uid(), ev );

    return true;
}
bool ToDoXMLResource::replaceEvent( const ToDoEvent& ev) {
    m_events.replace( ev.uid(), ev );

    return true;
}
bool ToDoXMLResource::removeEvent( const ToDoEvent& ev) {
    m_events.remove( ev.uid() );

    return true;
}
ToDoEvent ToDoXMLResource::todo( QAsciiDict<int>* dict, XMLElement* element )const {
    ToDoEvent ev;
    QMap<QString, QString> attributes = element->attributes();
    QMap<QString, QString>::Iterator it;

    int *find=0;
    int day, month, year;
    day = month = year = -1;
    for ( it = attributes.begin(); it != attributes.end(); ++it ) {
        find = (*dict)[ it.key() ];
        if (!find ) {
            qWarning("Unknown option" + it.key() );
            ev.setExtra( it.key(), it.data() );
            continue;
        }

        switch( *find ) {
        case ToDoEvent::Uid:
            ev.setUid( it.data().toInt() );
            break;
        case ToDoEvent::Category:
            ev.setCategories( QStringList::split(';', it.data() ) );
            break;
        case ToDoEvent::HasDate:
            ev.setHasDueDate( it.data().toInt() );
            break;
        case ToDoEvent::Completed:
            ev.setCompleted( it.data().toInt() );
            break;
        case ToDoEvent::Priority:
            ev.setPriority( it.data().toInt() );
            break;
        case ToDoEvent::DateDay:
            day = it.data().toInt();
            break;
        case ToDoEvent::DateMonth:
            month = it.data().toInt();
            break;
        case ToDoEvent::DateYear:
            year = it.data().toInt();
            break;
        case ToDoEvent::Progress:
            ev.setProgress( it.data().toInt() );
            break;
        case ToDoEvent::CrossReference:
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
                    ev.addRelated( (*strIt).left(pos),  (*strIt).mid(pos+1).toInt() );

            }
            break;
        }
        case ToDoEvent::HasAlarmDateTime:
            ev.setHasAlarmDateTime( it.data().toInt() );
            break;
        case ToDoEvent::AlarmDateTime: {
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
QString ToDoXMLResource::toString( const QArray<int>& ints ) const{
    QString str;

    for (uint i = 0; i < ints.size(); i++ ) {
        str += QString::number( i ) + ",";
    }
    str.remove( str.length() -1, 1 );
    return str;
}
QString ToDoXMLResource::toString( const ToDoEvent& ev )const {
    QString str;

    str += "Completed=\"" + QString::number( ev.isCompleted() ) + "\" ";
    str += "HasDate=\"" + QString::number( ev.hasDueDate() ) + "\" ";
    str += "Priority=\"" + QString::number( ev.priority() ) + "\" ";
    str += "Progress=\"" + Qtopia::escapeString( ev.summary() ) + "\" ";

    str += "Categories=\"" + toString( ev.categories() ) + "\" ";
    str += "Description=\"" + Qtopia::escapeString( ev.description() ) + "\" ";

    if ( ev.hasDueDate() ) {
        str += "DateYear=\"" + QString::number( ev.dueDate().year() ) + "\" ";
        str += "DateMonth=\"" + QString::number( ev.dueDate().month() ) + "\" ";
        str += "DateDay=\"" + QString::number( ev.dueDate().day() ) + "\" ";
    }
    str += "Uid=\"" + QString::number( ev.uid() ) + "\" ";

// append the extra options
    QMap<QString, QString> extras = ev.extras();
    QMap<QString, QString>::Iterator extIt;
    for (extIt = extras.begin(); extIt != extras.end(); ++extIt )
        str += extIt.key() + "=\"" +  extIt.data() + "\" ";

    // cross refernce
    QStringList list = ev.relatedApps();
    QStringList::Iterator listIt;
    QString refs;
    str += "CrossReference=\"";
    for ( listIt = list.begin(); listIt != list.end(); ++listIt ) {
        QArray<int> ints = ev.relations( (*listIt) );
        for ( uint i = 0; i< ints.count(); i++ ) {
            str += (*listIt) + "," + QString::number( i ) + ";";
        }
    }
    str = str.remove( str.length()-1, 1 );
    str += "\" ";

    str += "AlarmDateTime=\"" + TimeConversion::toISO8601( ev.alarmDateTime() ) + "\" ";

    return str;
}
/*
bool FileToDoResource::save( const QString& name,
                             const QValueList<ToDoEvent>& m_todos ) {
    XMLElement *tasks = new XMLElement( );
    tasks->setTagName("Tasks" );
    for( QValueList<ToDoEvent>::ConstIterator it = m_todos.begin(); it != m_todos.end(); ++it ){
	XMLElement::AttributeMap map;
	XMLElement *task = new XMLElement();
	map.insert( "Completed", QString::number((int)(*it).isCompleted() ) );
	map.insert( "HasDate", QString::number((int)(*it).hasDate() ) );
	map.insert( "Priority", QString::number( (*it).priority() ) );
        map.insert( "Progress", QString::number( (*it).progress() ) );
        map.insert( "Summary", (*it).summary() );
	QArray<int> arrat = (*it).categories();
	QString attr;
	for(uint i=0; i < arrat.count(); i++ ){
            attr.append(QString::number(arrat[i])+";" );
	}
	if(!attr.isEmpty() ) // remove the last ;
            attr.remove(attr.length()-1, 1 );
	map.insert( "Categories", attr );
	//else
	//map.insert( "Categories", QString::null );
	map.insert( "Description", (*it).description() );
	if( (*it).hasDate() ){
            map.insert("DateYear",  QString::number( (*it).date().year()  ) );
            map.insert("DateMonth", QString::number( (*it).date().month() ) );
            map.insert("DateDay", QString::number( (*it).date().day()  ) );
	}
	map.insert("Uid", QString::number( (*it).uid() ) );

        // append the extra options
        QMap<QString, QString> extras = (*it).extras();
        QMap<QString, QString>::Iterator extIt;
        for (extIt = extras.begin(); extIt != extras.end(); ++extIt )
            map.insert( extIt.key(),  extIt.data() );

*/
        // append the cross reference now
        /* get the list of cross referenced apps ( QString )
         * then get the coresponding QArray<int>
         * anf then append it like apps,id;apps,id
         *
         */
/*
        QStringList list = (*it).relatedApps();
        QStringList::Iterator listIt;
        QString refs;
        for (listIt = list.begin(); listIt != list.end(); ++listIt ) {
            QArray<int> ints = (*it).relations( (*listIt) );
            for (uint i = 0; i < ints.count(); i++ )
                refs+= (*listIt) + "," + QString::number( ints[i]) + ";";
        }
        map.insert("CrossReference", refs );

	// append alarm settings
	map.insert( "HasAlarmDateTime", QString::number((int)(*it).hasAlarmDateTime() ) );
	if( (*it).hasAlarmDateTime() ){
            map.insert("AlarmDateTime",  QString::number( (*it).alarmDateTime().time().hour() ) +
		       ":" + QString::number( (*it).alarmDateTime().time().minute() )+
		       ":" + QString::number( (*it).alarmDateTime().time().second() ) +
		       ";" + QString::number( (*it).alarmDateTime().date().day() ) +
		       ":" + QString::number( (*it).alarmDateTime().date().month() ) +
		       ":" + QString::number( (*it).alarmDateTime().date().year() ) );
	}

	task->setTagName("Task" );
	task->setAttributes( map );
	tasks->appendChild(task);
    }
    QFile file( name);
    if( file.open(IO_WriteOnly ) ){
        QTextStream stream(&file );
        stream.setEncoding( QTextStream::UnicodeUTF8 );
        stream << "<!DOCTYPE Tasks>" << endl;
        tasks->save(stream );
        delete tasks;
        stream << "</Tasks>" << endl;
        file.close();
        return true;
    }
    return false;
}
*/
