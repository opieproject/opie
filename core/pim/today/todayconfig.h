/*
                             This file is part of the Opie Project

                             Copyright (C) Maximilian Reiss <harlekin@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

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
class TodayConfigWidget;
class TodayPluginInterface;

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
    QMap<TodayPluginInterface*,TodayConfigWidget*> m_configMap;

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
