/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2009 Opie Developer Team <opie-devel@handhelds.org>
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

#include "kbddhandler.h"

/* Qt */
#include <qtextstream.h>

/* Std */
#include <stdlib.h>

void KbddHandler::setPort(const QString &port) {
    m_port = port;
}

void KbddHandler::setType(const QString &type) {
    m_type = type;
}

const QString &KbddHandler::getPort(void) {
    return m_port;
}

const QString &KbddHandler::getType(void) {
    return m_type;
}

void KbddHandler::readConfig(void) {
    QStringList contents;
    QFile conf( CONFIG_FILE );

    m_port = "";
    m_type = "";

    readFromFile(conf, contents);

    for (QStringList::Iterator it = contents.begin(); it != contents.end(); ++it ) {
        if((*it).startsWith("port:")) {
            m_port = (*it).mid(6);
        }
        else if((*it).startsWith("type:")) {
            m_type = (*it).mid(6);
        }
    }
}

void KbddHandler::writeConfig(void) {
    QStringList contents;
    QFile conf( CONFIG_FILE );

    readFromFile(conf, contents);

    bool foundport=false;
    bool foundtype=false;
    QString oldport, oldtype;
    for (QStringList::Iterator it = contents.begin(); it != contents.end(); ++it ) {
        if((*it).startsWith("port:")) {
            oldport = (*it).mid(6);
            (*it) = "port: " + m_port;
            foundport = true;
        }
        else if((*it).startsWith("type:")) {
            oldtype = (*it).mid(6);
            (*it) = "type: " + m_type;
            foundtype = true;
        }
    }
    if(!foundport)
        contents.append("port: " + m_port);
    if(!foundtype)
        contents.append("type: " + m_type);

    if(oldport != m_port || oldtype != m_type) {
        writeToFile(conf, contents);
    }
}

void KbddHandler::restart(void) {
    enableDisableSerialConsole(false);
    system(KBDD_SERVICECMD " stop");
    system(KBDD_SERVICECMD " start");
}

void KbddHandler::stop(void) {
    system(KBDD_SERVICECMD " stop");
    enableDisableSerialConsole(true);
}

void KbddHandler::enableDisableSerialConsole(bool enable) {
    QStringList contents;
    QFile conf(INIT_CONF);
    
    readFromFile(conf, contents);
    
    bool foundsetting = false;
    for (QStringList::Iterator it = contents.begin(); it != contents.end(); ++it ) {
        if((*it).contains("/sbin/getty")) {
            if(enable) {
                if((*it).startsWith("#")) {
                    (*it) = (*it).mid(1);
                    foundsetting = true;
                    break;
                }
            }
            else {
                if(!(*it).startsWith("#")) {
                    (*it) = "#" + (*it);
                    foundsetting = true;
                    break;
                }
            }
        }
    }
    
    if(foundsetting) {
        writeToFile(conf, contents);
        system("init q");
    }
}

bool KbddHandler::readFromFile(QFile &file, QStringList &strlist) {
    if(file.open(IO_ReadOnly)) {
        QTextStream in(&file);

        strlist = QStringList::split('\n', in.read(), true);
        file.close();
        if(strlist.count() > 0)
            strlist.remove(strlist.at(strlist.count() - 1));  // remove extra blank line
    }
    return true;
}

bool KbddHandler::writeToFile(QFile &file, QStringList &strlist) {
    if(file.open(IO_WriteOnly | IO_Truncate)) {
        QTextStream out(&file);
        for (QStringList::Iterator it = strlist.begin(); it != strlist.end(); ++it ) {
            out << (*it) << "\n";
        }
        file.close();
    }
    return true;
}
