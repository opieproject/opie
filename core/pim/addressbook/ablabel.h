/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#ifndef ABLABEL_H
#define ABLABEL_H

#include <opie/ocontact.h>
#include <qtextview.h>

#include <opie/ocontactaccess.h>

class AbLabel : public QTextView
{
    Q_OBJECT

public:
    AbLabel( QWidget *parent, const char *name = 0 );
    ~AbLabel();

    // Set the contacts 
    void setContacts( const OContactAccess::List& viewList );

    // Selects a contact 
    bool selectContact( int UID );

    // Get the UID of the current selected Entry
    int currentEntry_UID();

    //
    OContact currentEntry();

signals:
    void signalOkPressed();
    
protected:
    void sync();
    void keyPressEvent( QKeyEvent * );
    
private:
    OContactAccess::List m_viewList;
    OContactAccess::List::Iterator m_itCurContact;

    bool m_empty;

};

#endif // ABLABEL_H

