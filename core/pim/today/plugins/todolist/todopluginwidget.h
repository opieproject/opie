/*
 * todopluginwidget.h
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

#ifndef TODOLIST_PLUGIN_WIDGET_H
#define TODOLIST_PLUGIN_WIDGET_H

#include <qstring.h>
#include <qwidget.h>

#include <opie/otodo.h>
#include <opie/otodoaccess.h>
#include <opie/oclickablelabel.h>


class TodolistPluginWidget : public QWidget {

    Q_OBJECT

public:
    TodolistPluginWidget( QWidget *parent,  const char *name );
    ~TodolistPluginWidget();

protected slots:
    void  startTodolist();

private:
    OClickableLabel *todoLabel;

    OTodoAccess *todo;
    OTodoAccess::List m_list;
    OTodoAccess::List::Iterator m_it;

    void readConfig();
    void getTodo();
    int m_maxLinesTask;
    int m_maxCharClip;
};

#endif
