/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef SERVICEEDIT_H
#define SERVICEEDIT_H

#include "serviceeditbase.h"

#include <dbus/qdbusproxy.h>

class ConnManServiceEditor : public ConnManServiceEditorBase
{
    Q_OBJECT
public:
    ConnManServiceEditor( QDBusProxy *proxy, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ConnManServiceEditor();
protected:
    QDBusProxy *m_proxy;
    bool m_immutable;

    // Original values
    QString m_method4;
    QString m_method6;
    QMap<QString,QDBusData> m_ipv4map;
    QMap<QString,QDBusData> m_ipv6map;
    QMap<QString,QDBusData> m_proxymap;
    QString m_nameservers;
    QString m_domains;
    QString m_password;
    bool m_autoconnect;

    void accept();
protected slots:
    void slotV4ConfigChanged(int index);
    void slotV6ConfigChanged(int index);
    void slotProxyMethodChanged(int index);
};

#endif // SERVICEEDIT_H
