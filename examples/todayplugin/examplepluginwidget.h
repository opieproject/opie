/*
 * examplepluginwidget.h
 *
 * copyright   : (c) 2004 by Maximilian Reiﬂ
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

#ifndef EXAMPLE_PLUGIN_WIDGET_H
#define EXAMPLE_PLUGIN_WIDGET_H

#include <qlayout.h>

#include <opie/oclickablelabel.h>

class ExamplePluginWidget : public QWidget {

    Q_OBJECT

public:
    ExamplePluginWidget( QWidget *parent,  const char *name );
    ~ExamplePluginWidget();
    
    void refresh();

private slots:
   void slotClicked();

private:
    OClickableLabel* m_exampleLabel;
    QHBoxLayout* m_layout;
    void readConfig();
    void getInfo();
};

#endif
