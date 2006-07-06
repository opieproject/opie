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
 * The basic class for OBEX manipulating classes declaration
 */
#ifndef ObexBase_H
#define ObexBase_H

#include <qobject.h>

namespace Opie {namespace Core {class OProcess;}}
class QCopChannel;
namespace OpieObex {
    class ObexBase : public QObject {
      Q_OBJECT
    public:
        /**
         * ObexBase constructor look
         */
        ObexBase(QObject *parent, const char* name);
        /**
         * d'tor
         */
        virtual ~ObexBase();
        /**
         * Starting listening to an interface after enabled by the applet
         * a signal gets emitted when received a file
         */
        virtual void receive();
        /**
         * Send the file 
         * @param the name of the file
         * @param the address of the device
         */
        virtual void send(const QString&, const QString&);
        /**
         * Stop receiving
         * @param if true - does nothing if false - stops receiving
         */
        virtual void setReceiveEnabled(bool = false);
    signals:
        /**
         * Notify the upper level that we have received the file
         * @param path The path to the received file
         */
        void receivedFile(const QString& path);
        /**
         * error signal if the program couldn't be started or the
         * the connection timed out
         */
        void error(int);
        /**
         *  The current try to receive data
         */
        void currentTry(unsigned int);
        /**
         * signal sent The file got beamed to the remote location
         */
        void sent(bool);
        void done(bool);        
    protected:
        uint m_count;
        QString m_file;
        QString m_outp;
        QString m_bdaddr;
        bool m_receive : 1;
    protected slots:
        virtual void slotError();
    };
};    

#endif
//eof

