/*
 * todopluginwidget.h
 *
 * copyright   : (c) 2002, 2003 by Maximilian Reiﬂ
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

#include <qlayout.h>

#include <opie2/otodoaccess.h>
#include <opie2/oclickablelabel.h>


class TodolistPluginWidget : public QWidget {

    Q_OBJECT

public:
    TodolistPluginWidget( QWidget *parent,  const char *name );
    ~TodolistPluginWidget();

    void refresh();
    void reinitialize();

protected slots:
    void  startTodolist();

private:
    Opie::Ui::OClickableLabel *todoLabel;
    QVBoxLayout* layoutTodo;

    Opie::OPimTodoAccess *todo;
    Opie::OPimTodoAccess::List m_list;
    Opie::OPimTodoAccess::List::Iterator m_it;

    void readConfig();
    void getTodo();
    int m_maxLinesTask;
    int m_maxCharClip;
};

#endif
