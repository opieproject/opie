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

#ifndef DATEBOOK_PLUGIN_H
#define DATEBOOK_PLUGIN_H

#include <qstring.h>
#include <qwidget.h>

#include <opie/oclickablelabel.h>

#include "../../todayplugininterface.h"

class DatebookPlugin : public TodayPluginObject {

public:
    DatebookPlugin();
    ~DatebookPlugin();

    QString pluginName()  const;
    double versionNumber() const;
    QString pixmapNameWidget() const;
    QWidget* widget( QWidget *);
    QString pixmapNameConfig() const;
    TodayConfigWidget* configWidget( QWidget *);
    QString appName() const;

};


#endif
