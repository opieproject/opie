/*
 * todayconfig.h
 *
 * copyright   : (c) 2002,2003, 2004 by Maximilian Reiﬂ
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

#ifndef TODAYCONFIG_H
#define TODAYCONFIG_H

#include "todayconfigmiscbase.h"

#include <opie2/otabwidget.h>
#include <opie2/opluginloader.h>

#include <qvariant.h>
#include <qdialog.h>
#include <qlistview.h>

class QCheckBox;
class QLabel;
class QSpinBox;
class QTabWidget;

namespace Opie {
    namespace Core {
        class OPluginManager;
        class OPluginLoader;
        class OPluginItem;
    }
}

class TodayConfig : public QDialog {

    Q_OBJECT

public:
    TodayConfig( QWidget* parent = 0, const char* name = 0, bool modal = FALSE );
    ~TodayConfig();

    Opie::Ui::OTabWidget* TabWidget3;

    void writeConfig();

    void setUpPlugins( Opie::Core::OPluginManager * plugManager, Opie::Core::OPluginLoader * plugLoader );

protected slots:
    void moveSelectedUp();
    void moveSelectedDown();

private:
    void setAutoStart();
    void readConfig();
    void pluginManagement( Opie::Core::OPluginItem plugItem);

    QListView* m_appletListView;
    QMap<QString,QCheckListItem*> m_applets;

    int m_autoStart;
    int m_autoStartTimer;
    int m_iconSize;
    QStringList m_excludeApplets;
    bool m_applets_changed;

    QWidget* tab_2;
    QWidget* tab_3;
    QLabel* TextLabel1, *TextLabel4;
    QSpinBox* SpinBox7;

    Opie::Core::OPluginManager *m_pluginManager;
    Opie::Core::OPluginLoader *m_pluginLoader;

    QCheckListItem *previousItem;

    TodayConfigMiscBase *m_guiMisc;
};

#endif
