/*
 * stocktickerpluginwidget.h
 *
 * copyright   : (c) 2002 by L.J. Potter
 * email       : lornkcor@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef STOCKTICKERL_PLUGIN_WIDGET_H
#define STOCKTICKERL_PLUGIN_WIDGET_H

#include <qsocket.h>
#include <qstring.h>
#include <qwidget.h>
#include <qlineedit.h>

#include <opie/tododb.h>
#include <opie/oclickablelabel.h>
#include <opie/oticker.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class QTimer;

class StockTickerPluginWidget : public QWidget {

    Q_OBJECT

public:
    StockTickerPluginWidget( QWidget *parent,  const char *name );
    ~StockTickerPluginWidget();

protected slots:
    void doStocks();
    void isConnected();
    void socketError(int);
    void checkConnection();
private:
    int updateSpeed;
    QString symbollist;
    bool wasError;
    QSocket *Sock;
    void timerEvent( QTimerEvent * );
    void init();
//    void DefProxy(void);
    int timerDelay;
};

#endif
