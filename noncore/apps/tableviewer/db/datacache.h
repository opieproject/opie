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

#ifndef __DATACACHE_H__
#define __DATACACHE_H__

#include <qstring.h>
#include <qvector.h>
#include "common.h"

class DBStore;

/*! Abstract class that defines how database stuff can be accessed */
class DBAccess {
public:

//    DBAccess(DBStore *d) { dstore = d; }
    virtual ~DBAccess() {}

    virtual QString type() {
        return QString();
    }

    virtual bool openSource(QIODevice *) {
        qWarning("DBAccess::openSource not yet implemented");
        return false;
    }

    virtual bool saveSource(QIODevice *) {
        qWarning("DBAccess::saveSource(QString) not yet implemented");
        return false;
    }

protected:
    DBStore *dstore;
    QString source_name;
};

class DBStore {
public:
    DBStore();
    ~DBStore();

    bool openSource(QIODevice *, const QString &source);
    bool saveSource(QIODevice *, const QString &source);

    // Add an item
    void addItem(DataElem *);
    void addItemInternal(DataElem *);
	void removeItem(DataElem *);

    // Set the name of the database
    void setName(const QString &name);

    // Get the name of the database
    QString getName();

    KeyList *getKeys();
    void setKeys(KeyList *);

    /*! gets the number of fields defined in the database */
    inline int getNumFields() {
       return kRep->getNumFields();
    }

    /*! gets the index of a key given its name */
    inline int getKeyIndex(QString qs) {
        return kRep->getKeyIndex(qs);
    }

    /*! gets the type of a key given its index */
    inline TVVariant::KeyType getKeyType(int i) {
        return kRep->getKeyType(i);
    }

    /*! gets the name of a key given its index */
    inline QString getKeyName(int i) {
        return kRep->getKeyName(i);
    }

// Access functions.. iterator type stuff

    void first();
    void last();

	bool next();
	bool previous();

	DataElem* getCurrentData();

private:
    /* does the work of freeing used memory */
    void freeTable();
	QString name;

    QVector<DataElem> master_table;
    DBAccess *archive;

    KeyList *kRep;

    unsigned int number_elems;
    unsigned int table_size; /* should always be a power of 2 */
    bool full; /* since because we are using an int for indexing there is 
                  an upper limit on the number of items we can store. */
    unsigned int current_elem;
};
#endif
