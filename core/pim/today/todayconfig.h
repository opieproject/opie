/*
 * todayconfig.h
 *
 * copyright   : (c) 2002,2003 by Maximilian Rei�
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

#include <qvariant.h>
#include <qdialog.h>
#include <qlistview.h>
#include <opie/otabwidget.h>


#include "todayconfigmiscbase.h"

class QCheckBox;
class QLabel;
class QSpinBox;
class QTabWidget;

class TodayConfig : public QDialog {

    Q_OBJECT

public:
    TodayConfig( QWidget* parent = 0, const char* name = 0, bool modal = FALSE );
    ~TodayConfig();

    OTabWidget* TabWidget3;

    void writeConfig();
    void pluginManagement( QString libName, QString name, QPixmap icon );


protected slots:
    void appletChanged();
    void moveSelectedUp();
    void moveSelectedDown();

private:
    void setAutoStart();
    void readConfig();

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

    TodayConfigMiscBase *m_guiMisc;
};

#endif
