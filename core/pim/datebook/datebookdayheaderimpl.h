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
#ifndef DATEBOOKDAYHEADER_H
#define DATEBOOKDAYHEADER_H
#include "datebookdayheader.h"

#include <qdatetime.h>

class DateBookDayHeader : public DateBookDayHeaderBase
{
    Q_OBJECT

public:
    DateBookDayHeader( bool bUseMonday, QWidget* parent = 0,
		       const char* name = 0 );
    ~DateBookDayHeader();
    void setStartOfWeek( bool onMonday );

public slots:
    void goBack();
    void goForward();
    void setDate( int, int, int );
    void setDay( int );
    void gotHide();

signals:
    void dateChanged( int y, int m, int d );

private slots:
    void pickDate();


private:
    QDate currDate;
    bool bUseMonday;
    void setupNames();

};

#endif // DATEBOOKDAYHEADER_H
