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
#ifndef Tvkeyedit_H
#define Tvkeyedit_H

#include "tvkeyedit_gen.h"
#include "../db/common.h"

class TVKeyEdit : public TVKeyEdit_gen
{
	Q_OBJECT

signals:
    void listView();
    void browseView();

protected slots:
    void newTerm();
    void deleteTerm();
    void clearTerms();
    void updateTerm(int);
    void updateTerm(const QString &);
    void setTerm(QListViewItem *);

public:
	TVKeyEdit(TableState *ts, QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
	~TVKeyEdit();


    static KeyList *openEditKeysDialog(TableState *ts, QWidget *parent);

private:

    TableState *ts;
    int num_keys;
    KeyList working_state;
};

#endif
