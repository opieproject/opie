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

#include <sys/time.h>
#include <qwidget.h>

/*
  Little load meter
*/
class Load : public QWidget {
    Q_OBJECT
public:
    Load( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );

protected:
    void paintEvent( QPaintEvent *ev );

private slots:
    void timeout();

private:
    int points;
    double *userLoad;
    double *systemLoad;
    double maxLoad;
    struct timeval last;
    int lastUser;
    int lastUsernice;
    int lastSys;
    int lastIdle;
    bool first;
};

class LoadInfo : public QWidget
{
    Q_OBJECT
public:
    LoadInfo( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );

private:
    QPixmap makeLabel( const QColor &col, const QString &text );
    QString getCpuInfo();
};
