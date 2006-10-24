/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=           redistribute it and/or  modify it under
:=1 )Y*s>-.--   :            the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|     MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:      PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:        details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
/*
 * The Infrared OBEX handling class declaration
 */

#ifndef OpieObex_H
#define OpieObex_H

#include "obexbase.h"
#include <qobject.h>
#include "obexserver.h"

namespace Opie {namespace Core {class OProcess;}}
class QCopChannel;
namespace OpieObex {
  class Obex : public ObexBase {
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
       * a  signal gets emitted when received a file
       */
      virtual void receive();
      virtual void send(const QString& filename, const QString& addr);
      virtual void setReceiveEnabled( bool = false );
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
      Opie::Core::OProcess *m_send;
      ObexServer* m_rec; //The OBEX server
      bool m_receive : 1;
      void shutDownReceive();

private slots:

      /**
       * send over palm obex
       */

      //void send(const QString&);

      // the process exited
      void slotExited(Opie::Core::OProcess* proc) ;
      void slotStdOut(Opie::Core::OProcess*, char*, int);
      virtual void slotError();

  private:
      void sendNow();
      QString parseOut();
      void received();
      void sendEnd();

  };
};


#endif
