/*
                             This file is part of the Opie Project
                             Copyright (C) Paul Eggleton (bluelightning@bluelightning.org)
              =.             Portions Copyright (C) 2000-2002 Trolltech AS
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
#include <opie2/opimnotifymanager.h>
#include <opie2/opimrecurrence.h>
#include <opie2/opimtimezone.h>
#include <opie2/odatebookaccessbackend_vcal.h>
#include <opie2/odebug.h>

#include <qtopia/global.h>
#include <qtopia/stringutil.h>
#include <qtopia/timeconversion.h>

/* QT */
#include <qasciidict.h>
//FIXME: Hack to allow direct access to FILE* fh. Rewrite this!
// Good lord, this is absolutely hideous...
#define protected public
#include <qfile.h>
#undef protected

/* STD */
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <unistd.h>



using namespace Opie;

namespace {
    // Lifted from qpe/event.c

    // vcal conversion code
    static inline VObject *safeAddPropValue( VObject *o, const char *prop, const QString &value )
    {
        VObject *ret = 0;
        if ( o && !value.isEmpty() )
            ret = addPropValue( o, prop, value.latin1() );
        return ret;
    }

    static inline VObject *safeAddProp( VObject *o, const char *prop)
    {
        VObject *ret = 0;
        if ( o )
            ret = addProp( o, prop );
        return ret;
    }


    /*
    * Until we support vCal/iCal right
    * we will make DTSTART and other things
    * be floating in the sense of
    * RFC 2445
    */
    /*
    * Convert QDateTime to iso8601 but take
    * local time and do not use the Z at the end
    *
    */
    QCString toISOLocal( const QDateTime& dt ) {
        QCString str;
        /*
         * year month day T Hour Minute Second
         *  4    2     2     2    2       2    digits
         */
        str.sprintf("%04d%02d%02dT%02d%02d%02d",
                    dt.date().year(),
                    dt.date().month(),
                    dt.date().day(),
                    dt.time().hour(),
                    dt.time().minute(),
                    dt.time().second() );

        qWarning("Str ist %s", str.data() );

        return str;
    }


    // FIXME This is pretty incomplete, should probably extend it...
    static VObject *createVObject( const OPimEvent &e )
    {
        VObject *event = newVObject( VCEventProp );

        safeAddPropValue( event, VCDTstartProp, toISOLocal( e.startDateTime() ) );
        safeAddPropValue( event, VCDTendProp, toISOLocal( e.endDateTime() ) );
        safeAddPropValue( event, VCSummaryProp, e.description() );
        safeAddPropValue( event, VCDescriptionProp, e.note() );
        safeAddPropValue( event, VCLocationProp, e.location() );

        if ( e.hasNotifiers() ) {
            const OPimNotifyManager::Alarms &als = e.notifiers().alarms();
            OPimNotifyManager::Alarms::ConstIterator it;
            for ( it = als.begin(); it != als.end(); ++it ) {
                VObject *alarm = safeAddProp( event, VCAAlarmProp );
                safeAddPropValue( alarm, VCRunTimeProp, toISOLocal( (*it).dateTime() ) );
                safeAddPropValue( alarm, VCAudioContentProp,
                                ( (*it).sound() == OPimAlarm::Silent ? "silent" : "alarm" ) );
            }
        }

        if ( e.isAllDay() )
            safeAddPropValue( event, "X-Qtopia-AllDay", "TRUE" );
        else {
            // Only normal events have a timezone
            safeAddPropValue( event, "X-Qtopia-TIMEZONE", e.timeZone() );
        }

        // ### FIXME repeat missing

        safeAddPropValue( event, VCCategoriesProp, e.categoryNames( "Calendar" ).join(", ") );

        return event;
    }

    static OPimEvent parseVObject( VObject *obj )
    {
        OPimEvent e;

        bool haveAlarm = FALSE;
        bool haveStart = FALSE;
        bool haveEnd = FALSE;
        QDateTime alarmTime;
        OPimAlarm::Sound soundType = OPimAlarm::Loud;

        QString itemSummary, itemDesc, itemAttachNote;

        // We used to handle X-Qtopia-NOTES here, but since that seems to have
        // been set as a duplicate of the event description on vcal export (and
        // notes are not exported at all) it is useless and thus was removed.

        VObjectIterator it;
        initPropIterator( &it, obj );
        while( moreIteration( &it ) ) {
            VObject *o = nextVObject( &it );
            QCString name = vObjectName( o );
            QCString value = vObjectStringZValue( o );
            if ( name == VCDTstartProp ) {
                e.setStartDateTime( TimeConversion::fromISO8601( value ) );
                haveStart = TRUE;
            }
            else if ( name == VCDTendProp ) {
                e.setEndDateTime( TimeConversion::fromISO8601( value ) );
                haveEnd = TRUE;
            }
            else if ( name == VCSummaryProp ) {
                itemSummary = value;
            }
            else if ( name == VCDescriptionProp ) {
                itemDesc = value;
            }
            else if ( name == VCAttachProp ) {
                itemAttachNote = value;
            }
            else if ( name == VCLocationProp ) {
                e.setLocation( value );
            }
            else if ( name == VCAAlarmProp ) {
                haveAlarm = TRUE;
                VObjectIterator nit;
                initPropIterator( &nit, o );
                while( moreIteration( &nit ) ) {
                    VObject *o = nextVObject( &nit );
                    QCString name = vObjectName( o );
                    QCString value = vObjectStringZValue( o );
                    if ( name == VCRunTimeProp )
                        alarmTime = TimeConversion::fromISO8601( value );
                    else if ( name == VCAudioContentProp ) {
                        if ( value == "silent" )
                            soundType = OPimAlarm::Silent;
                        else
                            soundType = OPimAlarm::Loud;
                    }
                }
                e.notifiers().add( OPimAlarm( soundType, alarmTime, 0, e.uid(), alarmTime ) );
            }
            else if ( name == VCCategoriesProp ) {
                QStringList categories = QStringList::split( ",", value );
                // Palm likes to put X- on the start of category names
                for( QStringList::Iterator it = categories.begin(); it != categories.end(); ++it ) {
                    if( (*it).startsWith( "X-" ) )
                        (*it) = (*it).mid(2);
                }
                e.setCategoryNames( categories, "Calendar" ); // no tr
            }
            else if ( name == "X-Qtopia-TIMEZONE") {
                e.setTimeZone( value );
            }
            else if ( name == "X-Qtopia-AllDay" ) {
                e.setAllDay( true );
            }
#if 0
            else {
                printf("Name: %s, value=%s\n", name.data(), vObjectStringZValue( o ) );
                VObjectIterator nit;
                initPropIterator( &nit, o );
                while( moreIteration( &nit ) ) {
                    VObject *o = nextVObject( &nit );
                    QCString name = vObjectName( o );
                    QString value = vObjectStringZValue( o );
                    printf(" subprop: %s = %s\n", name.data(), value.latin1() );
                }
            }
#endif
        }

        if ( !haveStart && !haveEnd )
            e.setStartDateTime( QDateTime::currentDateTime() );

        if ( !haveEnd ) {
            e.setAllDay( true );
            e.setEndDateTime( e.startDateTime() ); // FIXME not sure this is correct
        }

        // Handle description and notes
        if( itemSummary.isEmpty() ) {
            // This is typical of vcs files sent from older PocketPC & Palm
            // summary is in description field and notes are an attachment
            e.setDescription( itemDesc );
            e.setNote( itemAttachNote );
        }
        else {
            // OTOH, KOrganiser stores events this way. Newer PocketPC versions
            // set the description the same as the summary and put notes in an
            // attachment.
            e.setDescription( itemSummary );
            if( itemAttachNote.isEmpty() )
                e.setNote( itemDesc );
            else {
                if( itemDesc.isEmpty() || itemDesc == itemSummary )
                    e.setNote( itemAttachNote );
                else {
                    // Just in case we have both
                    QString note = itemDesc;
                    note.append( '\n' );
                    note.append( itemAttachNote );
                    e.setNote( note );
                }
            }
        }

        return e;
    }
}

namespace Opie {

ODateBookAccessBackend_VCal::ODateBookAccessBackend_VCal( const QString& ,
                                                        const QString& fileName )
    : ODateBookAccessBackend() {
    m_file = fileName;
    m_changed = false;
}

ODateBookAccessBackend_VCal::~ODateBookAccessBackend_VCal() {
}

bool ODateBookAccessBackend_VCal::load() {
    m_changed = false;

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
        if( name == VCEventProp ){
            OPimEvent ev = parseVObject( vobj );
            if( ev.hasRecurrence() )
                m_rep.insert( ev.uid(), ev );
            else
                m_raw.insert( ev.uid(), ev );
        }
    }

    // FIXME Should I do a delete vcal?

    return true;
}

bool ODateBookAccessBackend_VCal::reload() {
    clear();
    return load();
}

bool ODateBookAccessBackend_VCal::save() {
    if (!m_changed)
        return true;

    QFile file( m_file );
    if (!file.open(IO_WriteOnly ) )
        return false;

    VObject *obj;
    obj = newVObject( VCCalProp );
    addPropValue( obj, VCVersionProp, "1.0" );
    VObject *vo;
    for(QMap<int, OPimEvent>::ConstIterator it=m_raw.begin(); it !=m_raw.end(); ++it ){
        vo = createVObject( it.data() );
        addVObjectProp(obj, vo );
    }
    for(QMap<int, OPimEvent>::ConstIterator it=m_rep.begin(); it !=m_rep.end(); ++it ){
        vo = createVObject( it.data() );
        addVObjectProp(obj, vo );
    }
    writeVObject( file.fh, obj ); //FIXME: HACK!!!
    cleanVObject( obj );
    cleanStrTbl();

    m_changed = false;
    return true;
}

QArray<int> ODateBookAccessBackend_VCal::allRecords()const {
    QArray<int> ints( m_raw.count()+ m_rep.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }
    for ( it = m_rep.begin(); it != m_rep.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}

QArray<int> ODateBookAccessBackend_VCal::queryByExample(const OPimEvent&, int,  const QDateTime& ) const {
    return QArray<int>();
}

void ODateBookAccessBackend_VCal::clear() {
    m_changed = true;
    m_raw.clear();
    m_rep.clear();
}

OPimEvent ODateBookAccessBackend_VCal::find( int uid ) const{
    if ( m_raw.contains( uid ) )
        return m_raw[uid];
    else
        return m_rep[uid];
}

bool ODateBookAccessBackend_VCal::add( const OPimEvent& ev ) {
    m_changed = true;
    if (ev.hasRecurrence() )
        m_rep.insert( ev.uid(), ev );
    else
        m_raw.insert( ev.uid(), ev );

    return true;
}

bool ODateBookAccessBackend_VCal::remove( int uid ) {
    m_changed = true;
    m_raw.remove( uid );
    m_rep.remove( uid );

    return true;
}

bool ODateBookAccessBackend_VCal::replace( const OPimEvent& ev ) {
    remove( ev.uid() );
    return add( ev );
}

QArray<int> ODateBookAccessBackend_VCal::rawRepeats()const {
    QArray<int> ints( m_rep.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_rep.begin(); it != m_rep.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}

QArray<int> ODateBookAccessBackend_VCal::nonRepeats()const {
    QArray<int> ints( m_raw.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}

OPimEvent::ValueList ODateBookAccessBackend_VCal::directNonRepeats()const {
    OPimEvent::ValueList list;
    QMap<int, OPimEvent>::ConstIterator it;
    for (it = m_raw.begin(); it != m_raw.end(); ++it )
        list.append( it.data() );

    return list;
}

OPimEvent::ValueList ODateBookAccessBackend_VCal::directRawRepeats()const {
    OPimEvent::ValueList list;
    QMap<int, OPimEvent>::ConstIterator it;
    for (it = m_rep.begin(); it != m_rep.end(); ++it )
        list.append( it.data() );

    return list;
}

}
