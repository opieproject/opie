/**********************************************************************
** SyslogInfo
**
** Display Syslog information
**
** Copyright (C) 2004, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
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

#ifndef SYSLOGINFO_H
#define SYSLOGINFO_H

#include <qwidget.h>

class QTextView;

class SyslogInfo : public QWidget
{
    Q_OBJECT
public:
    SyslogInfo( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~SyslogInfo();

private:
    QTextView* syslogview;
    int fd;

private slots:
    void updateData();
};

#endif
