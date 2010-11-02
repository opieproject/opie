/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2009 Opie Developer Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
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
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef KBDDHANDLER_H
#define KBDDHANDLER_H

/* Qt */
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>

#define CONFIG_FILE "/etc/kbdd.conf"
#define KBDD_SERVICECMD "/etc/init.d/kbdd"
#define INIT_CONF "/home/paul/tmp/ipaq/etc/inittab"

class KbddHandler {
protected:
    QString m_type;
    QString m_port;
    bool readFromFile(QFile &file, QStringList &strlist);
    bool writeToFile(QFile &file, QStringList &strlist);
public:
    const QString &getPort(void);
    const QString &getType(void);
    void setPort(const QString &port);
    void setType(const QString &type);
    void readConfig(void);
    void writeConfig(void);
    void restart(void);
    void stop(void);
    void enableDisableSerialConsole(bool enable);
};

#endif // KBDDHANDLER_H
