/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/ 
/*
 * This file is used to load the xml files that represent the database.
 * The main requirment for said file is each data entry must contain a key,
 * otherwise any other data headings are allowed.
 */

#include "datacache.h"
#include "xmlsource.h"
#include "csvsource.h"
#include <stdlib.h>
#include <qheader.h>

#define INIT_TABLE_SIZE 128

/*!
    \class DBStore datastore.h

    \brief The DBStore class is the class responsible for storing, sorting and
    searching the data loaded by the application

*/

/*!
    Constructs a DBStore item
*/
DBStore::DBStore()
{
    name = "";
    number_elems = 0;
    full = false;
    kRep = new KeyList();
    master_table.resize(INIT_TABLE_SIZE);
    table_size = INIT_TABLE_SIZE;

    current_elem = 0;
    archive = 0;
}

//TODO 
/*!
    Reinitializes the table to empty (include a resize of the master table, 
    which should free some memory) 
*/
void DBStore::freeTable()
{
    name = "";
    if(archive) {
        delete archive;
        archive = 0;
    }
    kRep->clear(); /* clear the current key list */

    number_elems = 0;
    table_size = INIT_TABLE_SIZE;
    master_table.resize(table_size);
    full = false;
    current_elem = 0;
}

/*!
    Removes all items from the DBStore and destroys the DBStore
*/
DBStore::~DBStore()
{
    freeTable();
}

/*!
    This function opens the given xml file, loads it and sets up the 
    appropriate data structures.

    \param file_name A string representing the name of the file to be opened
    \return true if successful, false otherwise.
*/
bool DBStore::openSource(QIODevice *inDev, const QString &source) {

	/* first check if db is already open, if contains data.. then clear */
    if(number_elems > 0) {
        freeTable();
    }

    if (source == "text/x-xml-tableviewer") {
        archive = new DBXml(this);
    } else if (source == "text/csv") {
	archive = new DBCsv(this);
    } else
    	return false;

    return (archive->openSource(inDev));
}

bool DBStore::saveSource(QIODevice *outDev, const QString &source) 
{
    /* saving a new file */
    if(!archive) {
        if (source == "text/x-xml-tableviewer") {
            archive = new DBXml(this);
	} else if (source == "text/x-xml-tableviewer") {
            archive = new DBCsv(this);
        } else
	    return false;
    }

    /* changing file type */
    if(archive->type() != source) {
        delete archive;
        if (source == "text/x-xml-tableviewer") {
            archive = new DBXml(this);
	} else if (source == "text/x-xml-tableviewer") {
            archive = new DBCsv(this);
        } else 
	    return false;
    }

    return (archive->saveSource(outDev));
}

/*! 
    This function is used to add new elements to the database.  If the database
    has already reached the maximum allowable size this function does not alter
    the database.

    \param delm An already allocated and initialized data element to be added
*/
void DBStore::addItem(DataElem *delem)
{
    addItemInternal(delem);
}

void DBStore::addItemInternal(DataElem *delem)
{
    /* if already full, don't over fill, do a qWarning though */
    if (full) {
        qWarning("Attempted to add items to already full table");
        return;
    }

    master_table.insert(number_elems, delem);

    current_elem = number_elems;
    number_elems++;

    if(number_elems >= table_size) {
        /* filled current table, double if we can */
        table_size = table_size << 1;

        /* check that the new table size is still valid, i.e. that we didn't
           just shift the 1 bit of the end of the int. */
        if (!table_size) {
            full = true;
            /* no point in doing antying else. */
            return;
        }
        master_table.resize(table_size);
    }
}

void DBStore::removeItem(DataElem *r)
{
    int position = master_table.findRef(r);
	if(position != -1) {
		/* there is at least one item, this is it */
		/* replace this with the last element, decrease the element count */
		master_table.insert(position, master_table.at(--number_elems));
		master_table.remove(number_elems);
		delete r;
	}
}

/*!
    Sets the name of the database

    \param n A string representing the new name of the database.
*/
void DBStore::setName(const QString &n)
{
	name = n;
}

/*!
    Gets the name of the database

    \return A string representing the name of the database.
*/
QString DBStore::getName()
{
	return name;
}

/*!
    Retrieves a pointer to the key representation of the database for 
    other classes to use as reference.

    \return a pointer to the databases key representaion
*/
KeyList *DBStore::getKeys()
{
    return kRep;
}

/*!
    sets the database's key representation the passed pointer
    \param a pointer to a key representaton
*/
void DBStore::setKeys(KeyList *k)
{
    kRep = k;
}

/*!
    Sets the current element to the first element of the database 
*/
void DBStore::first()
{
    current_elem = 0;
}

/*!
    Sets the current element to the last element of the database 
*/
void DBStore::last()
{
    current_elem = number_elems - 1;
}

/*!
    Sets the current element to the next element of the database if
    there exists an element after the current one.
*/
bool DBStore::next()
{
    unsigned int new_current_elem = current_elem + 1;
    if (current_elem < number_elems) 
        /* was valid before inc (it is possible but unlikely that inc current
           elem will change it from invalid to valid) */
        if (new_current_elem < number_elems) {
            /* is valid after inc */
            current_elem = new_current_elem;
            return true;
        }
    return false;
}

/*!
    Sets the current element to the previous element of the database if
    there exists an element before the current one.
*/
bool DBStore::previous()
{
    unsigned int new_current_elem = current_elem -1;
    if (current_elem < number_elems)
        /* was valid */
        if (new_current_elem < number_elems) {
            /* still is (if was 0, then now -1, but as is unsigned will wrap 
               and hence be invalid */
            current_elem = new_current_elem;
            return true;
        }
    return false;
}

/*!
    Returns the current data element in the database.  Which element is current
    is affected by newly added items, findItem, next, previous, first and 
    last functions

    \return a pointer to the current data element
*/
DataElem *DBStore::getCurrentData() 
{
    if (current_elem >= number_elems) 
        return NULL;
    return master_table[current_elem];
}
