/*
               =.            This file is part of the OPIE Project
             .=l.      Copyright (c)  2002 Holger Freyther <freyther@kde.org>
           .>+-=       the use of vobject was inspired by libkcal
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#include <qfile.h>
#include <qvaluelist.h>
#include <opie/todoevent.h>
#include <opie/todovcalresource.h>

#include "../library/backend/vobject_p.h"
#include "../library/backend/timeconversion.h"
#include "../library/backend/qfiledirect_p.h"

static VObject *vobjByEvent( const ToDoEvent &event )
{
  VObject *task = newVObject( VCTodoProp );
  if( task == 0 )
    return 0l;
  if( event.hasDate() )
    addPropValue( task, VCDueProp, TimeConversion::toISO8601( event.date() ) );

  if( event.isCompleted() )
    addPropValue( task, VCStatusProp, "COMPLETED");

  QString string = QString::number(event.priority() );
  addPropValue( task, VCPriorityProp, string.local8Bit() );
  addPropValue( task, VCCategoriesProp, event.allCategories().join(";").local8Bit() );
  addPropValue( task, VCDescriptionProp, event.description().local8Bit() );
  addPropValue( task, VCSummaryProp, event.summary().left(15).local8Bit() );
  return task;
};

static ToDoEvent eventByVObj( VObject *obj ){
  ToDoEvent event;
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
    event.setHasDate( true );
    name = vObjectStringZValue( ob );
    event.setDate( TimeConversion::fromISO8601( name).date() );
  }
  // categories
  if((ob = isAPropertyOf( obj, VCCategoriesProp )) != 0 ){
    name = vObjectStringZValue( ob );
    qWarning("Categories:%s", name.data() );
  }

  return event;
};


QValueList<ToDoEvent> ToDoVCalResource::load(const QString &file)
{
  QValueList<ToDoEvent> events;
  VObject *vcal = 0l;
  vcal = Parse_MIME_FromFileName( (char *)file.utf8().data()  ); // from vobject
  if(!vcal )
    return events;
  // start parsing

  VObjectIterator it;
  VObject *vobj;
  initPropIterator(&it, vcal);

  while( moreIteration( &it ) ) {
    vobj = ::nextVObject( &it );
    QCString name = ::vObjectName( vobj );
    //QCString objVal = ::vObjectStringZValue( vobj );
    // let's find out the type
    if( name == VCTodoProp ){
      events.append( eventByVObj( vobj ) );

    } // parse the value
  }
  return events;
}
bool ToDoVCalResource::save(const QString &fileName, const QValueList<ToDoEvent>&list )
{
  QFileDirect file ( fileName );
  if(!file.open(IO_WriteOnly ) )
    return false;
  // obj
  VObject *obj;
  obj = newVObject( VCCalProp );
  addPropValue( obj, VCVersionProp, "1.0" );
  VObject *vo;
  for(QValueList<ToDoEvent>::ConstIterator it = list.begin(); it != list.end(); ++it ){
    vo = vobjByEvent( (*it) );
    addVObjectProp(obj, vo );
  }
  writeVObject( file.directHandle(), obj );
  cleanVObject( obj );
  cleanStrTbl();

  return true;
}










