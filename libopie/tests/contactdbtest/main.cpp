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
//#include <mainwindow.h>
#include "opie/ocontactdb.h"
#include "qpe/contact.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{

    QPEApplication a( argc, argv );

    /* Testroutine für OContactDB */
    OContactDB contactdb ("testprg");
    Contact query;
    const Contact *result;

    QString queryString ("\\..*\\.20.*"); /* Every birthday in the year 20xx */
    query.setBirthday (queryString);
    if ( (result = contactdb.queryByExample (query, OContactDB::query_IgnoreCase | OContactDB::query_RegExp)) != NULL){
	    do{
		    qWarning ( "Found Name: %s", result->fullName().latin1() );
		    qWarning ( "Found UID:  %d", result->uid() );
		    qWarning ( "Birthday:   %s", result->birthday().latin1() );

		    result = contactdb.nextFound();
	    } while (result != NULL); 
    }



    return 0;
}
