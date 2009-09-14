/*
                             This file is part of the Opie Project
                             
              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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
/*
 * =====================================================================
 * ToDo: Text-Backend: Automatic reload if something was changed...
 *
 *
 */

#include <opie2/omemoaccess.h>
#include <opie2/obackendfactory.h>

/* OPIE */
#include <opie2/omemoaccessbackend_text.h>
#include <opie2/opimresolver.h>
#include <opie2/opimglobal.h>
#include <opie2/odebug.h>

//#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>

/* QT */
#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#include <qlist.h>
#include <qcopchannel_qws.h>

/* STD */
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


namespace Opie {

OPimMemoAccess::OPimMemoAccess ( const QString appname, const QString ,
                       OPimMemoAccessBackend* end, bool autosync ):
    OPimAccessTemplate<OPimMemo>( end )
{
    /* take care of the backend. If there is no one defined, we
    * will use the XML-Backend as default (until we have a cute SQL-Backend..).
    */
    if( end == 0 ) {
        end = OBackendFactory<OPimMemoAccessBackend>::defaultBackend( OPimGlobal::NOTES, appname );
    }
    // Set backend locally and in template
        m_backEnd = end;
    OPimAccessTemplate<OPimMemo>::setBackEnd (end);


    /* Connect signal of external db change to function */
    QCopChannel *dbchannel = new QCopChannel( "QPE/PIM", this );
    connect( dbchannel, SIGNAL(received(const QCString&,const QByteArray&)),
         this, SLOT(copMessage(const QCString&,const QByteArray&)) );
    if ( autosync ){
        QCopChannel *syncchannel = new QCopChannel( "QPE/Sync", this );
        connect( syncchannel, SIGNAL(received(const QCString&,const QByteArray&)),
             this, SLOT(copMessage(const QCString&,const QByteArray&)) );
    }


}
OPimMemoAccess::~OPimMemoAccess ()
{
    /* The user may forget to save the changed database, therefore try to
     * do it for him..
     */
    save();
    // delete m_backEnd; is done by template..
}


bool OPimMemoAccess::save ()
{
    /* If the database was changed externally, we could not save the
     * Data. This will remove added items which is unacceptable !
     * Therefore: Reload database and merge the data...
     */
    if ( OPimAccessTemplate<OPimMemo>::wasChangedExternally() )
        reload();

    bool status = OPimAccessTemplate<OPimMemo>::save();
    if ( !status )
        return false;

    /* Now tell everyone that new data is available.
     */
    QCopEnvelope e( "QPE/PIM", "notesUpdated()" );

    return true;
}

OPimChangeLog *OPimMemoAccess::changeLog() const
{
    return m_backEnd->changeLog();
}

#if 0
OPimRecordList<OPimMemo> OPimMemoAccess::sorted( bool ascending, int sortOrder, int sortFilter, int cat ) const
{
    QArray<int> matchingMemos = m_backEnd -> sorted( ascending, sortOrder, sortFilter, cat );
    return ( OPimRecordList<OPimMemo>(matchingMemos, this) );
}
#endif


bool OPimMemoAccess::wasChangedExternally()const
{
    return ( m_backEnd->wasChangedExternally() );
}


void OPimMemoAccess::copMessage( const QCString &msg, const QByteArray & )
{
    if ( msg == "notesUpdated()" ){
        emit signalChanged ( this );
    }
    else if ( msg == "flush()" ) {
        save ();
    }
    else if ( msg == "reload()" ) {
        reload ();
        emit signalChanged ( this );
    }
}

int OPimMemoAccess::rtti() const
{
    return OPimResolver::Notes;
}

}
