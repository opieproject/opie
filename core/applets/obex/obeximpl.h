
#ifndef OpieObexImpl_H
#define OpieObexImpl_H

#include <qobject.h>
#include "../obexinterface.h"
#include "obexdlg.h"
#include "obexinc.h"
#include <qpe/qcopenvelope_qws.h>

namespace OpieObex {
  class Obex;
  class ObexImpl  :  public QObject, public ObexInterface {
    Q_OBJECT
  public:
    ObexImpl();
    virtual ~ObexImpl();
    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT // for reference counting (macro )
  private:
    ulong ref;
    Obex* m_obex; // obex lib
    QCopChannel *m_chan;
    ObexDlg *m_sendgui;
    ObexInc *m_recvgui;
    QString m_name;
   private slots:
     void slotCancelSend();
     void slotMessage( const QCString&, const QByteArray& );
     void slotError(int );
      //  void slotCurrentTry( unsigned int ); */
      void slotDone(bool);
      void slotReceivedFile(const QString & );
      void slotSent();

  };
};
#endif
