/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

/* A Class to parse an comma seperated values docment of the form

"Displayed Name", key2name, key 3 name, key name 4
value1, , value 3, "value 4"
value1, , value 3, "value 4"

 * '\' is the escape character.  Basically \", \' and \\ escape the ", ' or \ 
 * into the actual value of the field. 
 * Before any other character will have the effect of dropping the '\'.
 *
 * Currently there is no support for comments. */

#ifndef __CSVSOURCE_H__
#define __CSVSOURCE_H__

#include <qstring.h>
#include "datacache.h"
#include "common.h"


class DBCsv : public DBAccess
{
public:
    /* create connection and either open or initialize */
    DBCsv(DBStore *d);
    QString type();
    bool openSource(QIODevice *);
    bool saveSource(QIODevice *);
    /* does a db write */
    ~DBCsv();
};

#endif
