#include <qfile.h>
#include <qvector.h>

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
    QAsciiDict<int> dict(21);
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
    dict.insert("State",           new int(OTodo::State)            );
    dict.insert("Recurrence",      new int(OTodo::Recurrence)       );
    dict.insert("Alarms",          new int(OTodo::Alarms)           );
    dict.insert("Reminders",       new int(OTodo::Reminders)        );
    dict.insert("Notifiers",       new int(OTodo::Notifiers)        );
    dict.insert("Maintainer",      new int(OTodo::Maintainer)       );

    // here the custom XML parser from TT it's GPL
    // but we want to push OpiePIM... to TT.....
    QFile f(m_file );
    if (!f.open(IO_ReadOnly) )
        return false;

    QByteArray ba = f.readAll();
    f.close();
    char* dt = ba.data();
    int len = ba.size();
    int i = 0;
    char *point;
    const char* collectionString = "<Task ";
    while ( dt+i != 0 && ( point = strstr( dt+i, collectionString ) ) != 0l ) {
        i = point -dt;
        i+= strlen(collectionString);
        OTodo ev;
        m_year = m_month = m_day = 0;

        while ( TRUE ) {
            while ( i < len && (dt[i] == ' ' || dt[i] == '\n' || dt[i] == '\r') )
		++i;
	    if ( i >= len-2 || (dt[i] == '/' && dt[i+1] == '>') )
		break;

	    // we have another attribute, read it.
	    int j = i;
	    while ( j < len && dt[j] != '=' )
		++j;
	    QCString attr( dt+i, j-i+1);

	    i = ++j; // skip =

	    // find the start of quotes
	    while ( i < len && dt[i] != '"' )
		++i;
	    j = ++i;

	    bool haveUtf = FALSE;
	    bool haveEnt = FALSE;
	    while ( j < len && dt[j] != '"' ) {
		if ( ((unsigned char)dt[j]) > 0x7f )
		    haveUtf = TRUE;
		if ( dt[j] == '&' )
		    haveEnt = TRUE;
		++j;
	    }
	    if ( i == j ) {
		// empty value
		i = j + 1;
		continue;
	    }

	    QCString value( dt+i, j-i+1 );
	    i = j + 1;

	    QString str = (haveUtf ? QString::fromUtf8( value )
		    : QString::fromLatin1( value ) );
	    if ( haveEnt )
		str = Qtopia::plainString( str );

            /*
             * add key + value
             */
            todo( &dict, ev, attr, str );

        }
        /*
         * now add it
         */
        if (m_events.contains( ev.uid() ) || ev.uid() == 0) {
            ev.setUid( 1 );
            m_changed = true;
        }
        if ( ev.hasDueDate() ) {
            ev.setDueDate( QDate(m_year, m_month, m_day) );
        }
        m_events.insert(ev.uid(), ev );
        m_year = m_month = m_day = -1;
    }

    qWarning("counts %d records loaded!", m_events.count() );
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
QArray<int> OTodoAccessXML::queryByExample( const OTodo&, int ) {
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
void OTodoAccessXML::todo( QAsciiDict<int>* dict, OTodo& ev,
                            const QCString& attr, const QString& val) {
//    qWarning("parse to do from XMLElement" );

    int *find=0;

    find = (*dict)[ attr.data() ];
    if (!find ) {
//            qWarning("Unknown option" + it.key() );
        ev.setCustomField( attr, val );
        return;
    }

    switch( *find ) {
    case OTodo::Uid:
        ev.setUid( val.toInt() );
        break;
    case OTodo::Category:
        ev.setCategories( ev.idsFromString( val ) );
        break;
    case OTodo::HasDate:
        ev.setHasDueDate( val.toInt() );
        break;
    case OTodo::Completed:
        ev.setCompleted( val.toInt() );
        break;
    case OTodo::Description:
        ev.setDescription( val );
        break;
    case OTodo::Summary:
        ev.setSummary( val );
        break;
    case OTodo::Priority:
        ev.setPriority( val.toInt() );
        break;
    case OTodo::DateDay:
        m_day = val.toInt();
        break;
    case OTodo::DateMonth:
        m_month = val.toInt();
        break;
    case OTodo::DateYear:
        m_year = val.toInt();
        break;
    case OTodo::Progress:
        ev.setProgress( val.toInt() );
        break;
    case OTodo::CrossReference:
    {
        /*
         * A cross refernce looks like
         * appname,id;appname,id
         * we need to split it up
         */
        QStringList  refs = QStringList::split(';', val );
        QStringList::Iterator strIt;
        for (strIt = refs.begin(); strIt != refs.end(); ++strIt ) {
            int pos = (*strIt).find(',');
            if ( pos > -1 )
                ; // ev.addRelation( (*strIt).left(pos),  (*strIt).mid(pos+1).toInt() );

        }
        break;
    }
    default:
        break;
    }
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


    return str;
}
QString OTodoAccessXML::toString( const QArray<int>& ints ) const {
    return Qtopia::Record::idsToString( ints );
}

/* internal class for sorting
 *
 * Inspired by todoxmlio.cpp from TT
 */

struct OTodoXMLContainer {
    OTodo todo;
};

namespace {
    inline QString string( const OTodo& todo) {
        return  todo.summary().isEmpty() ?
            todo.description().left(20 ) :
            todo.summary();
    }
    inline int completed( const OTodo& todo1, const OTodo& todo2) {
        int ret = 0;
        if ( todo1.isCompleted() ) ret++;
        if ( todo2.isCompleted() ) ret--;
        return ret;
    }
    inline int priority( const OTodo& t1, const OTodo& t2) {
        return ( t1.priority() - t2.priority() );
    }
    inline int description( const OTodo& t1, const OTodo& t2) {
        return QString::compare( string(t1), string(t2) );
    }
    inline int deadline( const OTodo& t1, const OTodo& t2) {
        int ret = 0;
        if ( t1.hasDueDate() &&
             t2.hasDueDate() )
            ret = t2.dueDate().daysTo( t1.dueDate() );
        else if ( t1.hasDueDate() )
            ret = -1;
        else if ( t2.hasDueDate() )
            ret = 1;
        else
            ret = 0;

        return ret;
    }

};

/*
   * Returns:
   *       0 if item1 == item2
   *
   *   non-zero if item1 != item2
   *
   *   This function returns int rather than bool so that reimplementations
   *   can return one of three values and use it to sort by:
   *
   *   0 if item1 == item2
   *
   *   > 0 (positive integer) if item1 > item2
   *
   *   < 0 (negative integer) if item1 < item2
   *
   */
class OTodoXMLVector : public QVector<OTodoXMLContainer> {
public:
    OTodoXMLVector(int size, bool asc,  int sort)
        : QVector<OTodoXMLContainer>( size )
        {
            setAutoDelete( true );
            m_asc = asc;
            m_sort = sort;
        }
        /* return the summary/description */
    QString string( const OTodo& todo) {
        return  todo.summary().isEmpty() ?
            todo.description().left(20 ) :
            todo.summary();
    }
    /**
     * we take the sortorder( switch on it )
     *
     */
    int compareItems( Item d1, Item d2 ) {
        bool seComp, sePrio, seDesc, seDeadline;
        seComp = sePrio = seDeadline = seDesc = false;
        int ret =0;
        OTodoXMLContainer* con1 = (OTodoXMLContainer*)d1;
        OTodoXMLContainer* con2 = (OTodoXMLContainer*)d2;

        /* same item */
        if ( con1->todo.uid() == con2->todo.uid() )
            return 0;

        switch ( m_sort ) {
            /* completed */
        case 0: {
            ret = completed( con1->todo, con2->todo );
            seComp = TRUE;
            break;
        }
            /* priority */
        case 1: {
            ret = priority( con1->todo, con2->todo );
            sePrio = TRUE;
            break;
        }
            /* description */
        case 2: {
            ret  = description( con1->todo, con2->todo );
            seDesc = TRUE;
            break;
        }
            /* deadline */
        case 3: {
            ret = deadline( con1->todo, con2->todo );
            seDeadline = TRUE;
            break;
        }
        default:
            ret = 0;
            break;
        };
        /*
         * FIXME do better sorting if the first sort criteria
         * ret equals 0 start with complete and so on...
         */

        /* twist it we're not ascending*/
        if (!m_asc)
            ret = ret * -1;

        if ( ret )
            return ret;

        // default did not gave difference let's try it other way around
        /*
         * General try if already checked if not test
         * and return
         * 1.Completed
         * 2.Priority
         * 3.Description
         * 4.DueDate
         */
        if (!seComp ) {
            if ( (ret = completed( con1->todo, con2->todo ) ) ) {
                if (!m_asc ) ret *= -1;
                return ret;
            }
        }
        if (!sePrio ) {
            if ( (ret = priority( con1->todo, con2->todo ) ) ) {
                if (!m_asc ) ret *= -1;
                return ret;
            }
        }
        if (!seDesc ) {
            if ( (ret = description(con1->todo, con2->todo ) ) ) {
                if (!m_asc) ret *= -1;
                return ret;
            }
        }
        if (!seDeadline) {
            if ( (ret = deadline( con1->todo, con2->todo ) ) ) {
                if (!m_asc) ret *= -1;
                return ret;
            }
        }

        return 0;
    }
 private:
    bool m_asc;
    int m_sort;

};

QArray<int> OTodoAccessXML::sorted( bool asc,  int sortOrder,
                                    int sortFilter,  int cat ) {
    qWarning("sorted! %d cat",  cat);
    OTodoXMLVector vector(m_events.count(), asc,sortOrder );
    QMap<int, OTodo>::Iterator it;
    int item = 0;

    bool bCat = sortFilter & 1 ? true : false;
    bool bOnly = sortFilter & 2 ? true : false;
    bool comp = sortFilter & 4 ? true : false;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {

        /* show category */
        if ( bCat && cat != 0)
            if (!(*it).categories().contains( cat ) ) {
                qWarning("category mis match");
                continue;
            }
        /* isOverdue but we should not show overdue - why?*/
/*        if ( (*it).isOverdue() &&  !bOnly  ) {
            qWarning("item is overdue but !bOnly");
            continue;
        }
*/
        if ( !(*it).isOverdue() && bOnly ) {
            qWarning("item is not overdue but bOnly checked");
            continue;
        }

        if ((*it).isCompleted() && comp ) {
            qWarning("completed continue!");
            continue;
        }


        OTodoXMLContainer* con = new OTodoXMLContainer();
        con->todo = (*it);
        vector.insert(item, con );
        item++;
    }
    qWarning("XXX %d Items added", item);
    vector.resize( item );
    /* sort it now */
    vector.sort();
    /* now get the uids */
    QArray<int> array( vector.count() );
    for (uint i= 0; i < vector.count(); i++ ) {
        array[i] = ( vector.at(i) )->todo.uid();
    }
    qWarning("array count = %d %d", array.count(), vector.count() );
    return array;
};
void OTodoAccessXML::removeAllCompleted() {
    for ( QMap<int, OTodo>::Iterator it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( (*it).isCompleted() )
            m_events.remove( it );
    }
}
