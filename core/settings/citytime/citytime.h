/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#ifndef TIMEWIDGETIMPL_H
#define TIMEWIDGETIMPL_H

#include <qlist.h>
#include <qwidget.h>

#include "citytimebase.h"

const int CITIES = 6;    // the number of cities...

class ZoneMap;

class CityTime : public CityTimeBase
{
    Q_OBJECT
public:
    CityTime(QWidget* parent = 0, const char *name = 0, WFlags fl = 0);
    ~CityTime();

public slots:
    void beginNewTz();
    void slotNewTz( const QString& strNewCountry, const QString& strNewCity );
    void changeClock( bool newClock );

protected:
    void timerEvent( QTimerEvent* );
    void mousePressEvent( QMouseEvent* event );

private:
    void showTime( void );  // get and show the time for various places...
    void readInTimes( void );   // a method to get information from the config
    QString strRealTz;  // save the TZ var
    QString nameRealTz; // and what it is called
    QString strHome;    // the home variable...
    bool bAdded;        // a flag to indicate things have been added...
    bool bWhichClock;
    int timerId;
    
    // a spot to hold the time zone for each city
    QString strCityTz[CITIES];
    QList<QToolButton> listCities;
    QList<QLabel> listTimes;
    bool changed;
};

#endif
