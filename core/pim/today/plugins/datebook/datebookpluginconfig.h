/*
 * datebookpluginconfig.h
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

#ifndef DATEBOOK_PLUGIN_CONFIG_H
#define DATEBOOK_PLUGIN_CONFIG_H


#include <qcheckbox.h>
#include <qspinbox.h>

#include <opie/todayconfigwidget.h>

class DatebookPluginConfig : public TodayConfigWidget {
    Q_OBJECT


public:
    DatebookPluginConfig( QWidget *parent,  const char *name );
   ~DatebookPluginConfig();

    void writeConfig();
private:
    /**
     * if changed then save
     */
    bool changed();
    void readConfig();

    QCheckBox* CheckBox2;
    QCheckBox* CheckBox1;
    QCheckBox* CheckBox3;
    QSpinBox* SpinBox1;
    QSpinBox* SpinBox2;


    // how many lines should be showed in the datebook section
    int m_max_lines_meet;
    // If location is to be showed too, 1 to activate it.
    int m_show_location;
    // if notes should be shown
    int m_show_notes;
    // should only later appointments be shown or all for the current day.
    int m_only_later;
    int m_more_days;

};


#endif
