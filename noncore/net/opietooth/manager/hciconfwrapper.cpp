#include "hciconfwrapper.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qregexp.h>

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
        qDebug ("NAME : " + name);
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
        } else {
            setValue( "auth" , "disable" );
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
        for (it = m_file.begin(); it != m_file.end(); ++it ) {
            str = (*it);
            if( (str.contains(key)) > 0 ) {
                qDebug("Found");
                // still need to look if its commented out!!!
                str.simplifyWhiteSpace();
                qDebug( key );
                if (str.startsWith("#")) {
                    str = (key + " " + value + ";");
                } else {
                    str = str.replace( QRegExp( "\\s*"+key+"\\s+[^\\s][^;]*;" ),  key + " " + value + ";");
                }
                qDebug( str );
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
        qWarning("loaded");
        m_file.clear();
        QFile file( m_fileName );
        if (!file.open( IO_ReadOnly ) ) {
            qDebug("Could not open");
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
        qWarning("save");
        if (m_file.isEmpty() ) // load first
            return;

        QFile file( m_fileName );
        if ( !file.open(IO_WriteOnly ) ) {
            qWarning("could not open %s",  m_fileName.latin1() );
            return;
        }

        QTextStream stream(&file );
        QStringList::Iterator it;
        for ( it = m_file.begin(); it != m_file.end(); ++it ) {
            stream << (*it) << endl;
        }
        qWarning("saved");
    };
}
