/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef NETWORK_H
#define NETWORK_H

#include <qstringlist.h>
class QListBox;
class QObject;
class QWidget;
struct NetworkInterface;
class Config;

class Network {
public:
    static void start(const QString& service=QString::null, const QString& password=QString::null);
    static void stop();

    static QString settingsDir();
    static QString serviceName(const QString& service);
    static QString serviceType(const QString& service);
    static bool serviceNeedsPassword(const QString& service);
    static void connectChoiceChange(QObject* receiver, const char* slot);
    static QStringList choices(QListBox* = 0, const QString& dir=QString::null);
    static void writeProxySettings( Config &cfg );

    static NetworkInterface* loadPlugin(const QString& type);

    static int addStateWidgets(QWidget* parent);

    // Server only
    static void createServer(QObject* parent);
    static bool networkOnline();
};

#endif
