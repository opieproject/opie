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
#ifndef Tvbrowseview_H
#define Tvbrowseview_H

#include "../db/common.h"
#include <qwidget.h>

class QTextBrowser;
class TVBrowseKeyEntry;

class TVBrowseView : public QWidget
{
	Q_OBJECT

signals:
    void searchOnKey(int keyIndex, TVVariant keyValue);
    void sortChanged(int);

public:
    TVBrowseView(TableState *t, QWidget* parent = 0, 
                 const char* name = 0, WFlags fl = 0);
    ~TVBrowseView();

    /* Access Methods */
    void setDisplayText(const DataElem *);
    void rebuildKeys();
    void rebuildData();
    void reset();

private:
    TableState *ts;

    QTextBrowser* textViewDisplay;
    TVBrowseKeyEntry *keyEntry;
};

#endif
