/*
 * addresspluginwidget.h
 *
 * copyright   : (c) 2003 by Stefan Eilers
 * email       : eilers.stefan@epost.de
 *
 * This implementation was derived from the todolist plugin implementation
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

#ifndef ADDRESSBOOK_PLUGIN_WIDGET_H
#define ADDRESSBOOK_PLUGIN_WIDGET_H

#include <opie2/ocontactaccess.h>
#include <opie2/oclickablelabel.h>

#include <qstring.h>
#include <qwidget.h>
#include <qlayout.h>


class AddressBookPluginWidget : public QWidget {

    Q_OBJECT

public:
    AddressBookPluginWidget( QWidget *parent,  const char *name );
    ~AddressBookPluginWidget();

    void reinitialize();
public slots:
    void refresh( const Opie::OPimContactAccess* db );

protected slots:
    void  startAddressBook();

private:
    Opie::Ui::OClickableLabel* addressLabel;
    QVBoxLayout* layoutTodo;
    Opie::OPimContactAccess * m_contactdb;

    Opie::OPimContactAccess::List m_list;
    Opie::OPimContactAccess::List::Iterator m_it;

    void readConfig();
    void getAddress();
    int m_maxLinesTask;
    int m_maxCharClip;
    int m_daysLookAhead;
    int m_urgentDays;
    QString m_entryColor;
    QString m_headlineColor;
    QString m_urgentColor;
    bool m_showBirthdays;
    bool m_showAnniversaries;
};

#endif
