/**********************************************************************
** Copyright (C) 2001 Devin Butterfield.  All rights reserved.
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

#ifndef CARDMON_H
#define CARDMON_H

#include <qwidget.h>
#include <qpixmap.h>

class CardMonitor : public QWidget {
    Q_OBJECT
public:
    CardMonitor( QWidget *parent = 0 );
    ~CardMonitor();
    bool getStatus( void );

private slots:
    void pcmciaMessage( const QCString &msg, const QByteArray & );

protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );
private:
    QPixmap pm;
    bool cardIn;
};

#endif // CARDMON_H

