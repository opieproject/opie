

#ifndef OpieBtObex_H
#define OpieBtObex_H

#include <qobject.h>
#include <services.h>
#include <manager.h>

namespace Opie {namespace Core {class OProcess;}}
class QCopChannel;
using namespace OpieTooth;
namespace OpieObex {
  // Maybe this should be derved from Obex.
  class BtObex : public QObject {
      Q_OBJECT
  public:
      /**
       * BtObex c'tor look
       */
      BtObex( QObject *parent, const char* name);
      /**
       * d'tor
       */
      ~BtObex();

      /** TODO mbhaynie -- Maybe opd would be a better way to receive.
       *  Starting listening to Bluetooth after enabled by the applet
       * a  signal gets emitted when received a file
       */
      void receive();
      void send( const QString&, const QString& );
      void setReceiveEnabled( bool = false );
  signals:

      /**
       * a signal
       * @param path The path to the received file
       */
      void receivedFile( const QString& path);
      /**
       * error signal if the program couldn't be started or the
       * the connection timed out
       */
      void error( int );
      /**
       *  The current try to receive data
       */
      void currentTry(unsigned int);
      /**
       * signal sent The file got beamed to the remote location
       */
      void sent(bool);
      void done(bool);

  private:
      uint m_count;
      QString m_file;
      QString m_outp;
      QString m_bdaddr;
      int m_port;
      Opie::Core::OProcess *m_send;
      Opie::Core::OProcess *m_rec;
      bool m_receive : 1;
      OpieTooth::Manager* btManager;
      void shutDownReceive();

private slots:

      // the process exited
      void slotExited(Opie::Core::OProcess*) ;
      void slotStdOut(Opie::Core::OProcess*, char*, int);
      void slotError();
      void slotFoundServices(const QString&, Services::ValueList);

  private:
      void sendNow();
      QString parseOut();
      void received();
      void sendEnd();

  };
};


#endif
