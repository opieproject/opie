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

#ifndef NEWTASKDIALOG_H
#define NEWTASKDIALOG_H

#include "todoentry.h"

#include <opie/otodo.h>
//#include <opie/ocontactselector.h>

#include <qdatetime.h>
#include <qpalette.h>

class QLabel;
class QTimer;
class DateBookMonth;



class NewTaskDialog : public NewTaskDialogBase
{
    Q_OBJECT

public:
    NewTaskDialog( const OTodo& task, QWidget *parent = 0, const char* name = 0,
		   bool modal = FALSE, WFlags fl = 0 );
    NewTaskDialog( int id, QWidget* parent = 0, const char* name = 0,
		   bool modal = FALSE, WFlags fl = 0 );
    ~NewTaskDialog();

    OTodo todoEntry();

protected slots:
    void dateChanged( int y, int m, int d );
    void groupButtonClicked ();
    void slotCopy();
protected:
    virtual void accept();

private:
    void init();
    OTodo todo;
    QDate date;
    DateBookMonth *picker;
};

#endif // NEWTASKDIALOG_H
