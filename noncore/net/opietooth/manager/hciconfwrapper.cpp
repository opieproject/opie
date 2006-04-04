/* $Id: hciconfwrapper.cpp,v 1.11 2006-04-04 12:16:09 korovkin Exp $ */
/* hcid.conf parser */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "hciconfwrapper.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>

#include <opie2/odebug.h>
using namespace Opie::Core;

namespace OpieTooth {


    HciConfWrapper::HciConfWrapper( const QString &fileName) {
        m_fileName = fileName;
    }

    HciConfWrapper::~HciConfWrapper() {
    }


    void HciConfWrapper::setPinHelper( const QString& app  ) {
        setValue( "pin_helper"  , app  );
    }

    void HciConfWrapper::setName( const QString &name ) {
        odebug << "NAME : " << name << oendl;
        setValue( "name" , "\"" +  name + "\"" );
    }

    void HciConfWrapper::setIscan( bool enable) {

        if ( enable ) {
            setValue( "iscan" , "enable" );
        } else {
            setValue( "iscan" , "disable" );
        }
    }

    void HciConfWrapper::setPscan( bool enable) {

        if ( enable ) {
            setValue( "pscan" , "enable" );
        } else {
            setValue( "pscan" , "disable" );
        }
    }


    void HciConfWrapper::setAuth( bool enable) {

        if ( enable ) {
            setValue( "auth" , "enable" );
            setValue( "security" , "auto" );
        } else {
            setValue( "auth" , "disable" );
            setValue( "security" , "none" );
        }
    }


    void HciConfWrapper::setEncrypt( bool enable) {

        if ( enable ) {
            setValue( "encrypt" , "enable" );
        } else {
            setValue( "encrypt" , "disable" );
        }
    }


    void HciConfWrapper::setValue(const QString &key, const  QString &value ) {

        if (m_file.isEmpty() ) // load first
            return;

        QStringList::Iterator it;

        QString str;
        QString tmpLine;
        bool wasCommented = false; //If the optin string was commented out
        QRegExp rx1("^" + key + "[ ]+"); //Regexp fo key searching
        QRegExp rx2(";[ ]*" + key + "[ ]+"); //Regexp fo key searching
        for (it = m_file.begin(); it != m_file.end(); ++it ) {
            wasCommented = false;
            str = (*it);
            tmpLine = str.simplifyWhiteSpace();
            //If it's commented out, remove the comment and check again
            //Now, let's check if this is a real keyword (not word_keyword)
            if (tmpLine.startsWith("#")) { 
                tmpLine.remove(0, 1);
                tmpLine = tmpLine.simplifyWhiteSpace();
                wasCommented = true;
            }
            if( (tmpLine.contains(rx1)) > 0 || (tmpLine.contains(rx2)) > 0) {
                odebug << "Found " + key << oendl;
                
                if (wasCommented)
                    str = ("\t" + key + " " + value + ";");
                else
                    str = str.replace(QRegExp("\\s*" + key + "\\s+[^\\s][^;]*;"), 
                        "\t" + key + " " + value + ";");
                odebug << str << oendl;
                it = m_file.remove( it );
                it = m_file.insert( it,  str );
                //return; the regexp is too wide  -zecke // all set
            }
        }
    }

    /**
     * This loads the config file and stores it inside
     * the m_file
     */
    void HciConfWrapper::load() {
        owarn << "loaded" << oendl;
        m_file.clear();
        QFile file( m_fileName );
        if (!file.open( IO_ReadOnly ) ) {
            odebug << "Could not open" << oendl;
            return;
        }

            /**
             * readAll() into a QByteArray
             * QStringList::split('\n', array )
             * would this be faster? -zecke
             */
            QTextStream stream(&file );
            QString tmp;
            while ( !stream.atEnd() ) {
                tmp = stream.readLine();
                m_file.append( tmp );
            }
    }
    void HciConfWrapper::save() {
        owarn << "save" << oendl;
        if (m_file.isEmpty() ) // load first
            return;

        QFile file( m_fileName );
        if ( !file.open(IO_WriteOnly ) ) {
            owarn << "could not open " << m_fileName.latin1() << "" << oendl;
            return;
        }

        QTextStream stream(&file );
        QStringList::Iterator it;
        for ( it = m_file.begin(); it != m_file.end(); ++it ) {
            stream << (*it) << endl;
        }
        owarn << "saved" << oendl;
    };
}
