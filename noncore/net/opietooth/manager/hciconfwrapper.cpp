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


    void HciConfWrapper::setPinHelper( QString app  ) {
        setValue( "pin_helper"  , app  );
    }

    void HciConfWrapper::setName( QString name ) {
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

        QFile f( m_fileName );
        QFile f2( m_fileName );

        if ( !f.open( IO_ReadOnly) ) {
            qDebug("Could not open readonly");
            return;
        }

        if ( !f2.open( IO_ReadWrite ) ) {
            qDebug("Just readonly - not enough");
            return;
        }

        QStringList list;
        qDebug(m_fileName);
        QTextStream stream ( &f);
        QTextStream outstream (&f2);

        QString str;
        while ( !(str=stream.readLine()).isNull() ) {


            //qDebug(str);
            if( (str.contains(key)) > 0 ) {
                qDebug("Found");
                // still need look if its commented out!!!
                str.simplifyWhiteSpace();
                qDebug( key );
                if (str.startsWith("#")) {
                    outstream << (key + " " + value + ";\n");
                } else {
                    str.replace( QRegExp( "\\s*"+key+"\\s+[^\\s][^;]*;" ),  key + " " + value + ";\n");
                }

                qDebug( str );
                outstream << str;
            } else {
                outstream << str + "\n";
            }
        }

        f.close();
        f2.flush();
        f2.close();
    }

}
