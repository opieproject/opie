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

#ifndef SDMON_H
#define SDMON_H

#include <qwidget.h>
#include <qimage.h>

class SdMonitor : public QWidget {
    Q_OBJECT
public:
    SdMonitor( QWidget *parent = 0 );
    ~SdMonitor();
    bool getStatus( void );

private slots:
    void cardMessage( const QCString &msg, const QByteArray & );

protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );
private:
    QImage img;
    bool cardIn;
};

#endif // SDMON_H





