
#ifndef OpieObexImpl_H
#define OpieObexImpl_H

#include "../obexinterface.h"
#include <qpe/qcopenvelope_qws.h>

namespace OpieObex {
  class Obex;
  class ObexImpl  : public ObexInterface, public QObject {
    Q_OBJECT
  public:
    ObexImpl();
    virtual ObexImpl();
    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT // for reference counting (macro )
  private:
    ulong ref;
    Obex* m_obex; // obex lib
    QCopChannel *m_chan;
   private slots:
     void slotMessage( const QCString&, const QByteArray& );
  };
};
#endif
