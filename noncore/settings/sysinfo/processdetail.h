/**********************************************************************
** ProcessDetail
**
** Display process information
**
** Copyright (C) 2002, Dan Williams
**                    williamsdr@acm.org
**                    http://draknor.net
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

#ifndef PROCESSDETAIL_H
#define PROCESSDETAIL_H

#include <qdialog.h>
#include <qcombo.h>
#include <qtextview.h>
#include <qpushbutton.h>
#include <qlistview.h>

class ProcessDetail : public QDialog
{
    Q_OBJECT

public:
    ProcessDetail( QWidget* parent,  const char* name, bool modal, WFlags fl );
    ~ProcessDetail();

    QComboBox* SignalCB;
    QTextView* ProcessView;
    QPushButton* SendButton;

    int pid;

private slots:
    void slotSendClicked();
};

#endif // PROCESSDETAIL_H
