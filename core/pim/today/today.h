/*
 * today.h
 *
 * copyright   : (c) 2002,2003 by Maximilian Reiﬂ
 * email       : harlekin@handhelds.org
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


#ifndef TODAY_H
#define TODAY_H

#include <qscrollview.h>
#include <qvbox.h>

#include <qpe/qlibrary.h>

#include "todayconfig.h"
#include "todaybase.h"

#include <opie/todayplugininterface.h>

class QVBoxLayout;
class OClickableLabel;


class Today : public TodayBase {

    Q_OBJECT

  public:
    Today( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Today();

private slots:
    void startConfig();
    void startApplication();
    void editCard();
    void refresh();

private:
    void init();
    void setOwnerField();
    void setOwnerField(QString &string);
    void loadPlugins();
    void draw();
    void reinitialize();

    void setRefreshTimer( int );

private slots:
    void channelReceived(const QCString &msg, const QByteArray & data);

 private:
    TodayConfig *conf;
    QStringList m_excludeApplets;
    QStringList m_allApplets;

    QScrollView *sv;
    QVBox* big_box;


    QTimer *m_refreshTimer;

    bool m_refreshTimerEnabled;
    int m_newStart;
    int m_iconSize;
    int m_maxCharClip;
    int m_hideBanner;
};

#endif
