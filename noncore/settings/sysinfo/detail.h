/**********************************************************************
** ModulesDetail
**
** Display module information
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** Based on ProcessDetail by Dan Williams <williamsdr@acm.org>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef DETAIL_H
#define DETAIL_H

#include <qwidget.h>
#include <qcombo.h>
#include <qtextview.h>
#include <qpushbutton.h>
#include <qlistview.h>

class Detail : public QWidget
{
    Q_OBJECT

public:
    Detail( QWidget * = 0x0,  const char * = 0x0, WFlags = 0 );
    ~Detail();

    QTextView* detailView;
};

#endif // DETAIL_H
