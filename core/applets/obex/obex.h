

#ifndef OpieObex_H
#define OpieObex_H

#include <qobject.h>

class OProcess;
class QCopChannel;
namespace OpieObex {
  class Obex : public QObject {
      Q_OBJECT
  public:
      /**
       * Obex c'tor look
       */
      Obex( QObject *parent, const char* name);
      /**
       * d'tor
       */
      ~Obex();

      /**
       *  Starting listening to irda after enabled by the applet
       * a  signal gets emitted when recieved a file
       */
      void receive();
      void send( const QString& );
      void setReceiveEnabled( bool = false );
  signals:

      /**
       * a signal
       * @param path The path to the recieved file
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
      void sent();
      // private slots
      void done(bool);

  private:
      uint m_count;
      QString m_file;
      QString m_outp;
      OProcess *m_send;
      OProcess *m_rec;
      bool m_receive : 1;
      void shutDownReceive();

private slots:

      /**
       * send over palm obex
       */

      //void send(const QString&);

      // the process exited
      void slotExited(OProcess* proc) ;
      void slotStdOut(OProcess*, char*, int);
      void slotError();

  private:
      void sendNow();
      QString parseOut();
      void recieved();
      void sendEnd();

  };
};


#endif
