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

#include <qstring.h>
#include <qwidget.h>
#include <qlayout.h>

#include <opie/ocontactaccess.h>
#include <opie/oclickablelabel.h>


class AddressBookPluginWidget : public QWidget {

    Q_OBJECT

public:
    AddressBookPluginWidget( QWidget *parent,  const char *name );
    ~AddressBookPluginWidget();

protected slots:
    void  startAddressBook();
    void refresh( const OContactAccess* db ); 

private:
    OClickableLabel* addressLabel;
    QVBoxLayout* layoutTodo;
    OContactAccess * m_contactdb;

    OContactAccess::List m_list;
    OContactAccess::List::Iterator m_it;

    void readConfig();
    void getAddress();
    int m_maxLinesTask;
    int m_maxCharClip;
    int m_daysLookAhead;
    QString m_entryColor;
};

#endif
