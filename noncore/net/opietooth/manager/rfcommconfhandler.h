#ifndef RFCOMMCONFHANDLER
#define RFCOMMCONFHANDLER

#include <qstring.h>
#include <qfile.h>
#include <qobject.h>
#include <qmap.h>

namespace OpieTooth {

    class RfCommConfObject {

    public:
        RfCommConfObject( int number, QString mac, int channel, QString comment, bool bind);
        ~RfCommConfObject();

        void setNumber( int number );
        int number() { return m_number; };
        void setMac( QString mac );
        QString mac() { return m_mac; };
        void setChannel( int channel );
        int channel() { return m_channel; };
        void setComment( QString comment );
        QString comment() { return m_comment; };
        bool isBind() { return m_doBind; }
        void doBind(bool arg) { m_doBind = arg; }

    private:
        bool m_doBind;
        int m_number;
        QString m_mac;
        int m_channel;
        QString m_comment;

    };

    class RfCommConfHandler  {

    public:
        RfCommConfHandler( const QString & filename );
        ~RfCommConfHandler();

        void save( QMap<QString, RfCommConfObject*> devices );

        QMap<QString, RfCommConfObject*> foundEntries();

    private:
        void load();

        QString m_filename;
        QMap<QString, RfCommConfObject*> m_foundEntries;
    };

}

#endif
