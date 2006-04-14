/* $Id: forwarder.h,v 1.1 2006-04-14 19:22:37 korovkin Exp $
 *	Bluetooth serial forwarder class declaration
 *
 *	(c) Copyright 2006 GPL
 *
 *	This software is provided under the GNU public license, incorporated
 *	herein by reference. The software is provided without warranty or 
 *	support.
 */
#ifndef OpieTooth_Forwarder_H
#define OpieTooth_Forwarder_H
#include <qobject.h>
#include <qstring.h>
#include <unistd.h>
#include <signal.h>
#include <opie2/oprocess.h>
namespace Opie {
    namespace Core {
        class OProcess;
        namespace Internal {
            class OProcessController; 
        }
    }
};

namespace OpieTooth {
    class SerialForwarder : public Opie::Core::OProcess {
        
        Q_OBJECT
    
    protected:
        QString device; //Name of the device
        int speed; //device speed to set
    public:
        SerialForwarder(QString& devName, int speed);
        ~SerialForwarder();
        //Function starts the forwarding process
        virtual bool start( RunMode runmode = NotifyOnExit,
                        Communication comm = NoCommunication );
        //Stop the forwarding process
        int stop();
    };
};
#endif
//eof
