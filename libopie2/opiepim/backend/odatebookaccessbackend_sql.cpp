/*
 * SQL Backend for the OPIE-Calender Database.
 *
 * Copyright (c) 2003 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 * =====================================================================
 * Version: $Id: odatebookaccessbackend_sql.cpp,v 1.1 2003-12-08 15:18:12 eilers Exp $
 * =====================================================================
 * History:
 * $Log: odatebookaccessbackend_sql.cpp,v $
 * Revision 1.1  2003-12-08 15:18:12  eilers
 * Committing unfinished sql implementation before merging to libopie2 starts..
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <qarray.h>
#include <qstringlist.h>

#include "orecur.h"
#include "odatebookaccessbackend_sql.h"

#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>

namespace {



};

ODateBookAccessBackend_SQL::ODateBookAccessBackend_SQL( const QString& ,
                                                        const QString& fileName )
	: ODateBookAccessBackend(), m_driver( NULL ) 
{
    m_fileName = fileName.isEmpty() ? Global::applicationFileName( "datebook", "datebook.db" ) : fileName;

    // Get the standart sql-driver from the OSQLManager..
    OSQLManager man;
    m_driver = man.standard();
    m_driver->setUrl( m_fileName );

    initFields();

    load();
}

ODateBookAccessBackend_SQL::~ODateBookAccessBackend_SQL() {
}

void ODateBookAccessBackend_SQL::initFields()
{

	// This map contains the translation of the fieldtype id's to 
	// the names of the table columns
	m_fieldMap.insert( OEvent::FUid, "uid" );
	m_fieldMap.insert( OEvent::FCategories, "Categories" );
	m_fieldMap.insert( OEvent::FDescription, "Description" );
	m_fieldMap.insert( OEvent::FLocation, "Location" );
	m_fieldMap.insert( OEvent::FType, "Type" );
	m_fieldMap.insert( OEvent::FAlarm, "Alarm" );
	m_fieldMap.insert( OEvent::FSound, "Sound" );
	m_fieldMap.insert( OEvent::FRType, "RType" );
	m_fieldMap.insert( OEvent::FRWeekdays, "RWeekdays" );
	m_fieldMap.insert( OEvent::FRPosition, "RPosition" );
	m_fieldMap.insert( OEvent::FRFreq, "RFreq" );
	m_fieldMap.insert( OEvent::FRHasEndDate, "RHasEndDate" );
	m_fieldMap.insert( OEvent::FREndDate, "REndDate" );
	m_fieldMap.insert( OEvent::FRCreated, "RCreated" );
	m_fieldMap.insert( OEvent::FRExeptions, "RExceptions" );
	m_fieldMap.insert( OEvent::FStart, "Start" );
	m_fieldMap.insert( OEvent::FEnd, "End" );
	m_fieldMap.insert( OEvent::FNote, "Note" );
	m_fieldMap.insert( OEvent::FTimeZone, "TimeZone" );
	m_fieldMap.insert( OEvent::FRecParent, "RecParent" );
	m_fieldMap.insert( OEvent::FRecChildren, "Recchildren" );
}

bool ODateBookAccessBackend_SQL::load() 
{
	if (!m_driver->open() )
		return false;

	// Don't expect that the database exists.
	// It is save here to create the table, even if it
	// do exist. ( Is that correct for all databases ?? )
	QString	qu = "create table datebook( uid INTEGER PRIMARY KEY ";
	
	QMap<int, QString>::Iterator it;
	for ( it = ++m_fieldMap.begin(); it != m_fieldMap.end(); ++it ){
		qu += QString( ",\"%1\" VARCHAR(10)" ).arg( it.data() );
	}
	qu += " );";
		
	qu += "create table custom_data( uid INTEGER, id INTEGER, type VARCHAR, priority INTEGER, value VARCHAR, PRIMARY KEY /* identifier */ (uid, id) );";

	OSQLRawQuery raw( qu );
	OSQLResult res = m_driver->query( &raw );
	if ( res.state() != OSQLResult::Success )
		return false;

	update();

	return true;
}

void ODateBookAccessBackend_SQL::update()
{

	QString qu = "select uid from datebook";
	OSQLRawQuery raw( qu );
	OSQLResult res = m_driver->query( &raw );
	if ( res.state() != OSQLResult::Success ){
		m_uids.clear();
		return;
	}
	
	m_uids = extractUids( res );
	
}

QArray<int> ODateBookAccessBackend_SQL::extractUids( OSQLResult& res ) const
{
	qWarning("extractUids");

	OSQLResultItem::ValueList list = res.results();
	OSQLResultItem::ValueList::Iterator it;
	QArray<int> ints(list.count() );
	qWarning(" count = %d", list.count() );

	int i = 0;
	for (it = list.begin(); it != list.end(); ++it ) {
		ints[i] =  (*it).data("uid").toInt();
		i++;
	}

	return ints;

}

bool ODateBookAccessBackend_SQL::reload() 
{
    return load();
}

bool ODateBookAccessBackend_SQL::save() 
{
	return m_driver->close();
}

QArray<int> ODateBookAccessBackend_SQL::allRecords()const 
{
    return m_uids;
}

QArray<int> ODateBookAccessBackend_SQL::queryByExample(const OEvent&, int,  const QDateTime& ) {
    return QArray<int>();
}

void ODateBookAccessBackend_SQL::clear() 
{
	QString qu = "drop table datebook;";
	qu += "drop table custom_data;";

	OSQLRawQuery raw( qu );
	OSQLResult res = m_driver->query( &raw );

}


OEvent ODateBookAccessBackend_SQL::find( int uid ) const{
}

bool ODateBookAccessBackend_SQL::add( const OEvent& ev ) {
    return true;
}
bool ODateBookAccessBackend_SQL::remove( int uid ) {

    return true;
}
bool ODateBookAccessBackend_SQL::replace( const OEvent& ev ) {
    remove( ev.uid() );
    return add( ev );
}
QArray<int> ODateBookAccessBackend_SQL::rawEvents()const {
    return allRecords();
}
QArray<int> ODateBookAccessBackend_SQL::rawRepeats()const {

    return ints;
}
QArray<int> ODateBookAccessBackend_SQL::nonRepeats()const {

    return ints;
}
OEvent::ValueList ODateBookAccessBackend_SQL::directNonRepeats() {

    return list;
}
OEvent::ValueList ODateBookAccessBackend_SQL::directRawRepeats() {

    return list;
}


QArray<int> ODateBookAccessBackend_SQL::matchRegexp(  const QRegExp &r ) const
{

    return m_currentQuery;
}
