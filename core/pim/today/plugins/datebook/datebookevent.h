/*
 * datebookplugin.h
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
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

#ifndef DATEBOOKEVENT_PLUGIN_H
#define DATEBOOKEVENT_PLUGIN_H

#include <opie/oclickablelabel.h>
#include <qpe/datebookdb.h>


class DateBookEvent: public OClickableLabel {

     Q_OBJECT

public:
  DateBookEvent( const EffectiveEvent &ev,
                 QWidget* parent = 0,
                 bool show_location = 0,
                 bool show_notes = 0,
                 int maxCharClip = 0,
                 const char* name = 0,
                 WFlags fl = 0 );
signals:
    void editEvent( const Event &e );
private slots:
    void editEventSlot( const Event &e );
    void editMe();
private:
    DateBookDB *db;
    QString ampmTime( QTime );
    QString differDate( QDate date );
    const EffectiveEvent event;
    bool ampm;
};

#endif
