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

#ifndef MODULESDETAIL_H
#define MODULESDETAIL_H

#include <qwidget.h>
#include <qcombo.h>
#include <qtextview.h>
#include <qpushbutton.h>
#include <qlistview.h>

class ModulesDetail : public QWidget
{
    Q_OBJECT

public:
    ModulesDetail( QWidget* parent,  const char* name, WFlags fl );
    ~ModulesDetail();

    QComboBox* CommandCB;
    QTextView* ModulesView;
    QPushButton* SendButton;

    QString modname;

private slots:
    void slotSendClicked();
};

#endif // MODULESDETAIL_H
