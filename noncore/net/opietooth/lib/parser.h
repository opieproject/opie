
#ifndef OpieToothParser_H
#define OpieToothParser_H

#include <services.h>

namespace OpieTooth {
    class Parser{
    public:
        Parser(const QString& output );
        ~Parser() {};
        void setText(const QString& output );
        Services::ValueList services()const;
    private:
        Services::ValueList m_list;
        Services m_item;
        void parse( const QString& );
        bool parseName(const QString& );
        bool parseRecHandle( const QString& );
        bool parseClassId( const QString& );
        bool parseProtocol( const QString& id );
        bool parseProfile( const QString& ) ;
        bool m_complete:1;
        bool m_ok;
        bool m_classOver:1;
        bool m_profOver:1;
        bool m_protocolOver:1;
        bool m_protocolAdded:1;
        QString m_protName;
        int m_protId;
        QString m_profName;
        int m_profId;
    };
};


#endif
