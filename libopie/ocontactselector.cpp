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
 * ToDo: There is a lot todo.. It is currently very simplistic..
 * =====================================================================
 * Version: $Id: ocontactselector.cpp,v 1.1.2.4 2002-07-05 11:17:19 zecke Exp $
 * =====================================================================
 * History:
 * $Log: ocontactselector.cpp,v $
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
#include <qdialog.h>
#include <qvbox.h>
#include <qlayout.h>

#include "ocontactselector.h"
#include "ocontactdb.h"

// using namespace Opie;

// namespace {

ONameItem::ONameItem( QTable *t, const QString fullname, const QString firstname,
		      const QString middlename, const QString lastname, const int uid ):
	QTableItem (t, QTableItem::Never, fullname),
	FullName (fullname),
	LastName (lastname),
	FirstName (firstname),
	MiddleName (middlename),
	sortkey (Qtopia::buildSortKey ( lastname + firstname + middlename )),
	Uid (uid),
	selected (false)
{
}

void ONameItem::toggle()
{
	selected = !selected;
}


OContactSelector::OContactSelector (QWidget * parent):
	QTable (0, 2, parent),
	contactdb ("todolist")
{
	setSorting ( TRUE );
	setSelectionMode ( NoSelection );
	setColumnWidth( 0, 20 );
	setColumnStretchable ( 1, TRUE );
	setLeftMargin (0);
	verticalHeader()->hide();
	horizontalHeader()->setLabel (0, tr( "C." ) );
	horizontalHeader()->setLabel (1, tr( "Contact" ) );

	connect( this, SIGNAL( clicked( int, int, int, const QPoint & ) ),
		 this, SLOT( slotClicked( int, int, int, const QPoint & ) ) );

	/* Fill the table with all entries of the contact database */
	QValueList<Contact> allContacts = contactdb.allContacts();
	QValueListIterator<Contact> it;
	setNumRows (allContacts.count());
	int row = 0;
	for( it = allContacts.begin(); it != allContacts.end(); ++it ){
		ONameItem *nameitem = new ONameItem (this,
						     (*it).fullName(),
						     (*it).firstName(),
						     (*it).middleName(),
						     (*it).lastName(),
						     (*it).uid() );
		OCheckItem *checkitem = new OCheckItem (this, nameitem->key());
		setItem ( row, 0, checkitem );
		setItem ( row, 1, nameitem );
		++row;
	}

	sortColumn (0, TRUE, TRUE);

}


OContactSelector::~OContactSelector (){}

ONameItem *OContactSelector::getItemAtRow (int row)const
{
	return ((ONameItem *) item (row, 1));
}

OCheckItem *OContactSelector::getCheckItemAtRow (int row)
{
	return ((OCheckItem *) item (row, 0));
}

void OContactSelector::slotClicked( int row, int col, int, const QPoint &pos )
{
	qWarning ("Executing SlotClicked");

	if ( !cellGeometry( row, col ).contains(pos) )
		return;
	// let's switch on the column number...
	switch ( col )
		{
		case 0: {
			OCheckItem *i = static_cast<OCheckItem*>(item( row, col ));
			if ( i ) {
				int x = pos.x() - columnPos( col );
				int y = pos.y() - rowPos( row );
				int w = columnWidth( col );
				int h = rowHeight( row );
				// printf ("Boxsize: %d = 10 ??\n",OCheckItem::BoxSize);
				if ( i && x >= ( w - OCheckItem::BoxSize ) / 2
				     && x <= ( w - OCheckItem::BoxSize ) / 2 + OCheckItem::BoxSize
				     && y >= ( h - OCheckItem::BoxSize ) / 2
				     && y <= ( h - OCheckItem::BoxSize ) / 2 + OCheckItem::BoxSize ) {
					i->toggle();
					updateCell( row, col );
					getItemAtRow (row) -> toggle();
				}
				// emit signalDoneChanged( i->isChecked() );
			}
		}
			break;
		case 1:
			break;
		}
}

QValueList<int> OContactSelector::selected ()const
{
	QValueList<int> listOfSelectedContacts;
	ONameItem *curritem = 0l;
	for (int i = 0; i < numRows(); i++){
		curritem = getItemAtRow (i);
		if (curritem){
			if (curritem -> isSelected())
				listOfSelectedContacts.append(curritem -> uid());
		}
	}
	return (listOfSelectedContacts);
}

void OContactSelector::setSelected (const QValueList<int> &selectedList)
{
	// OK, we need two nested loops to find the selected users..
	// To be as fast as possible, we will use the given "selected List" as
	// inner loop, expecting it is much smaller than the list of contacts.

	ONameItem *curritem = NULL;
	int currentuid = 0;
        QValueListConstIterator<int> it;

	for (int row = 0; row < numRows(); row++){
		curritem = getItemAtRow (row);
		if (curritem){
			for( it = selectedList.begin(); it != selectedList.end(); ++it ){
				currentuid = (*it);
				if (currentuid == curritem -> uid()){
					curritem -> toggle();
					getCheckItemAtRow (row) -> toggle();
					break;
				}
			}
		}
	}


}


OContactSelectorDialog::OContactSelectorDialog( QWidget* , WFlags fl )
	: QDialog (0 , tr ("Select Group Members"), true , fl)
{
	// resize( 240, 321 );
        QVBoxLayout* vb = new QVBoxLayout(this,1);
	contacttable = new OContactSelector(this);
	vb->add(contacttable);
}

QValueList<int> OContactSelectorDialog::selected ()const
{
	return (contacttable -> selected());
}
void OContactSelectorDialog::setSelected (const QValueList<int> &selectedList){
	contacttable -> setSelected (selectedList);
}

OContactSelectorDialog::~OContactSelectorDialog()
{
	delete (contacttable);
}

// } // End of namespace Opie
