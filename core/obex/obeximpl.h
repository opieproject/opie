#ifndef OPIE_OBEX_IMPL_QUERY_H
#define OPIE_OBEX_IMPL_QUERY_H

#include <obexinterface.h>

namespace OpieObex {
    class ObexHandler;
    class ObexImpl : public ObexInterface {
    public:
        ObexImpl();
        virtual ~ObexImpl();
        QRESULT queryInterface( const QUuid&, QUnknownInterface** );
        Q_REFCOUNT

    private:
        ObexHandler *m_handler;

    };
};

#endif
