/*
 * This class reads the addressbook and creates a selectable
 * list of contacts. 
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public 
 *      License as published by the Free Software Foundation; either 
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 * ToDo: ...
 * =====================================================================
 * Version: $Id: ocontactselector.h,v 1.1.2.4 2002-07-05 11:17:19 zecke Exp $
 * =====================================================================
 * History:
 * $Log: ocontactselector.h,v $
 * Revision 1.1.2.4  2002-07-05 11:17:19  zecke
 * Some API updates by me
 *
 * Revision 1.1.2.3  2002/07/03 06:55:54  eilers
 * Moved to LGPL
 *
 * Revision 1.1.2.2  2002/07/02 16:36:47  eilers
 * changed interface
 *
 * Revision 1.1.2.1  2002/07/01 16:49:46  eilers
 * First attempt for cross reference
 *
 */
#ifndef _OCONTACTSELECTOR_H
#define _OCONTACTSELECTOR_H
#include <qtable.h>
#include <qdialog.h>
#include <qpe/stringutil.h>

#include "ocontactdb.h"
#include "ocheckitem.h"

/** Class represents one contact entry.
 */
class ONameItem: public QTableItem
{
 public:
	/** Contructor of a contact entry.
	 * @param t Pointer to the contact table
	 * @param fullname The full name as it should be shown in the table
	 * @param firstname The first name of the contact
	 * @param middlename The second name of the contact
	 * @param lastname The lastname of the contact
	 * @param uid The unique User ID of the contact
	 */
	ONameItem ( QTable *t, const QString fullname, const QString firstname, 
		    const QString middlename, const QString lastname, const int uid );

	/** Returns the sortkey of this entry. This is used by the QTable class.
	 */
	QString key () const { return sortkey; }

	/** Returns the text which is shown in the table */
	QString text() const { return FullName; }

	/** Returns the user id of this entry */
	int uid() const { return Uid; }

	/** Toggle selection */
	void toggle();

	/** Returns if selected */
	bool isSelected() const { return selected; }

 private:
	QString FullName;
	QString LastName;
	QString FirstName;
	QString MiddleName;
	QString sortkey;
	int Uid;
	bool selected : 1;
};

/** This Class creates a table with all available contacts.
 * Future: The category should be selectable. And searching would be nice ...
 */
class OContactSelector: public QTable 
{
    Q_OBJECT

 public:
       /** Constructor.
	*/
       OContactSelector (QWidget * parent = 0);
	~OContactSelector ();

	/** Get all selected contacts.
	 * @returns List of user id's from selected users
	 */
 	QValueList<int> selected () const;

	/** Set selected contacts.
	 * The given list will select the contacts in the table with
	 * the same user id.
	 * @param selectedList List of user id's 
	 */
	void setSelected (const QValueList<int> &selectedList);

 private:
	OContactDB contactdb;
	ONameItem *getItemAtRow (int row)const;
	OCheckItem *getCheckItemAtRow (int row);

 private slots:
	void slotClicked( int row, int col, int button, const QPoint &pos );

 

};

/** Simple Dialog window.
 * Future: Add search and category field .
 */
class OContactSelectorDialog: public QDialog 
{

 public:
	OContactSelectorDialog ( QWidget* parent = 0, WFlags fl = 0 );
	~OContactSelectorDialog();

	/** Get all selected contacts.
	 * @see OContactSelector
	 */
	QValueList<int> selected () const;

	/** Set selected contacts.
	 * @see OContactSelector
	 */
	void setSelected (const QValueList<int> &selectedList);


 signals:
	// void signalSelected ( const QValueList<int> & );

 public slots:
	 // void reload ();

 private:
	OContactSelector *contacttable;


};


#endif
