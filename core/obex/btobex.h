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
 * The Bluetooth OBEX manipulating class declaration
 */


#ifndef OpieBtObex_H
#define OpieBtObex_H
#ifdef BLUETOOTH

#include "obexbase.h"
#include <qobject.h>
#include <services.h>
#include <manager.h>
#include <obexpush.h>
#include "obexserver.h"

namespace Opie {namespace Core {class OProcess;}}
class QCopChannel;
using namespace OpieTooth;

namespace OpieObex {
    // Maybe this should be derved from Obex.
    class BtObex : public ObexBase {
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
        int m_port;
        ObexPush* m_send;
        bool m_receive : 1;
        OpieTooth::Manager* btManager;
        void shutDownReceive();
        ObexServer* m_rec;

    private slots:

        // Push process slots
        void slotPushStatus(QCString&);
        void slotPushComplete(int);
        void slotPushError(int);

        // the process exited
        void slotExited(Opie::Core::OProcess*) ;
        void slotStdOut(Opie::Core::OProcess*, char*, int);
        void slotError();
        void slotFoundServices(const QString&, Services::ValueList);

    private:
        void sendNow();
        QString parseOut();
        void received();
    };
};

#endif //BLUETOOTH
#endif
