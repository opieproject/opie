#include <qasciidict.h>
#include <qfile.h>
#include <qmap.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <opie/xmltree.h>

#include "todoxmlresource.h"


using namespace Opie;

QValueList<ToDoEvent>  FileToDoResource::load( const QString& name ) {
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
    QValueList<ToDoEvent> m_todos;
    XMLElement *root = XMLElement::load( name );
    int day, year, month;
    day = year = month = -1;
    if(root != 0l ){ // start parsing
        qWarning("ToDoDB::load tagName(): %s", root->tagName().latin1()  );
	//if( root->tagName() == QString::fromLatin1("Tasks" ) ){// Start
        XMLElement *element = root->firstChild();
        if (element == 0 )
            return m_todos;
        element = element->firstChild();
        while( element ){
            if( element->tagName() != QString::fromLatin1("Task") ){
                element = element->nextChild();
                continue;
            }
            qWarning("ToDoDB::load element tagName() : %s", element->tagName().latin1() );
            QString dummy;
            ToDoEvent event;
            QMap<QString, QString> attributes = element->attributes();
            QMap<QString, QString>::Iterator it;
            int* find;
            for ( it = attributes.begin(); it != attributes.end(); ++it ) {
                find = dict[ it.key() ];
                /*
                 * it's any extraoption
                 */
                if (!find ) { // what could that be
                    qWarning("Unknown option %s", it.key().latin1() );
                    event.setExtra( it.key(), it.data() );
                    continue;
                }
                /*
                 * Check for our option
                 */
                switch (*find) {
                case ToDoEvent::Uid:
                    event.setUid( it.data().toInt() );
                    break;
                case ToDoEvent::Category:
                    event.setCategories( QStringList::split(';', it.data() ) );
                    break;
                case ToDoEvent::HasDate:
                    event.setHasDate( it.data().toInt() );
                    break;
                case ToDoEvent::Completed:
                    event.setCompleted( it.data().toInt() );
                    break;
                case ToDoEvent::Description:
                    event.setDescription( it.data() );
                    break;
                case ToDoEvent::Summary:
                    event.setSummary( it.data() );
                    break;
                case ToDoEvent::Priority:
                    event.setPriority( it.data().toInt() );
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
                    event.setProgress( it.data().toInt() );
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
                            event.addRelated( (*strIt).left(pos),  (*strIt).mid(pos+1).toInt() );

                    }
                    break;
                }
		case ToDoEvent::HasAlarmDateTime:
                    event.setHasAlarmDateTime( it.data().toInt() );
                    break;
		case ToDoEvent::AlarmDateTime:{
			/*
			 * The Time string looks like this:
			 * HH:MM:SS;DD:MM:YYYY
			 * We have to parse it...
			 * Zecke: Do you know a smarter solution ? (se)
			 */
			QStringList timedate = QStringList::split( ';', it.data() );
			QStringList::Iterator elements = timedate.begin();

			/* Parse timestring */
			QStringList refs = QStringList::split( ':', (*elements) );
			QStringList::Iterator strIt = refs.begin();
			int hours   = 0;
			int minutes = 0;
			int seconds = 0;
			int day     = 0;
			int month   = 0;
			int year    = 0;

			for (int count = 0; count < 3; count++){ 
				switch (count){
				case 0:
					hours = (*strIt).toInt();
					break;
				case 1:
					minutes = (*strIt).toInt();
					break;
				case 2:
					seconds = (*strIt).toInt();
					break;
				}
				++strIt;
			}

			/* Parse Datestring */
			if ( elements++ != timedate.end() ){
				refs = QStringList::split( ':', (*elements) );
				strIt = refs.begin();

				for (int count = 0; count < 3; count++){ 
					switch (count){
					case 0:
						day = (*strIt).toInt();
						break;
					case 1:
						month = (*strIt).toInt();
						break;
					case 2:
						year = (*strIt).toInt();
						break;
					}
					++strIt;
				}
			}

			QTime alarmTime( hours, minutes, seconds );
			QDate alarmDate( year, month, day );
			QDateTime alarmDateTime ( alarmDate, alarmTime );
			event.setAlarmDateTime( alarmDateTime );
		}
			break;
                default:
                    break;
                }
            }
            if (event.hasDate() ) {
                QDate date( year, month, day );
                event.setDate( date );
            }
            m_todos.append( event );
            element = element->nextChild(); // next element
        }
        //}
    } else {
        qWarning("could not load" );
    }
    delete root;
    qWarning("returning" );
    return m_todos;
}
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


        // append the cross reference now
        /* get the list of cross referenced apps ( QString )
         * then get the coresponding QArray<int>
         * anf then append it like apps,id;apps,id
         *
         */
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
