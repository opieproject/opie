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
 * The OBEX server class declaration
 * Based on OBEX server from GPE (thanks, guys)
 */
#ifndef ObexServer_H
#define ObexServer_H

#include <qobject.h>
#include <opie2/oprocess.h>

#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <openobex/obex.h>

namespace Opie {
    namespace Core {
        class OProcess;
        namespace Internal {
            class OProcessController;
        }
    }
};

namespace Opie {namespace Core {class OProcess;}}
namespace OpieObex {
    class ObexServer : public Opie::Core::OProcess {
        Q_OBJECT
    private:
	int transport; //The OBEX transport type
    public:
        /**
         * ObexServer constructor
         */
        ObexServer(int trans);
        /**
         *
         */
        ~ObexServer();
        //Function starts the server process
        virtual bool start( RunMode runmode = NotifyOnExit,
                        Communication comm = NoCommunication );
        //Stop the server process
        int stop();
    protected: //variables
        obex_t* m_obex; //Obex server handler
        sdp_session_t* m_session; //SDP session handler;
    protected: //functions
        //Funtion initializes obex server return 0 on success and -1 on error
        int initObex(void);
        //Function registers an OBEX push service
        sdp_session_t* addOpushSvc(uint8_t chan, const char* name);
    signals:
    protected slots:
    };
};

#endif
