#ifndef HCICONFWRAPPER_H
#define HCICONFWRAPPER_H

#include <qstring.h>

namespace OpieTooth {

    class HciConfWrapper {

    public:
        HciConfWrapper( const QString &fileName );
        ~HciConfWrapper();

        void setPinHelper( QString app  );
        void setName( QString name );
        void setIscan( bool enable );
        void setPscan( bool enable );
        void setAuth( bool enable);
        void setEncrypt( bool enable);

    private:

        void setValue(const QString &entry, const  QString &value );

        QString m_fileName;
    };

}

#endif
