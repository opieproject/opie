/**********************************************************************
** ModulesInfo
**
** Display modules information
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** Based on ProcessInfo by Dan Williams <williamsdr@acm.org>
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

#ifndef MODULESINFO_H
#define MODULESINFO_H

#include <qwidget.h>

class QComboBox;
class QListView;

class ModulesInfo : public QWidget
{
    Q_OBJECT
public:
    ModulesInfo( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~ModulesInfo();

private:
    QListView* ModulesView;
    QComboBox* CommandCB;

private slots:
    void updateData();
    void slotSendClicked();
};

#endif
