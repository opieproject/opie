/* $Id: obexpush.h,v 1.1 2006-05-10 13:32:46 korovkin Exp $ */
/* OBEX push functions declarations */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _OBEXPUSH_H_
#define _OBEXPUSH_H_
#include <qobject.h>
#include <qstring.h>
#include "services.h"
#include <opie2/oprocess.h>
namespace OpieTooth {
    class ObexPush : public QObject {
        Q_OBJECT
    public:
        /**
         * Constructor which creates an object
         */
        ObexPush();
        
        /**
         * Public destructor
         */
        ~ObexPush();
        
        /**
         * Function that sends a file
         * @param mac destination device MAC address
         * @param port service port number
         * @param src source file name
         * @param dst destination file name
         * @return 0 on success, -1 on error, 1 if sending process is running
         */
        int send(QString& mac, int port, QString& src, QString& dst);

        /**
         * @return true if it's sending and false otherwise
         */
         bool isSending() { return pushProc->isRunning(); }
    signals:
        /**
         * Informs that the sending process has completed
         * @param status the finish status
         */
        void sendComplete(int);

        /**
         * Informs that the sending process has finished with error
         * @param status the finish status
         */
        void sendError(int);
        
        /**
         * Informs that we have a string status update
         * @param str status string
         */
        void status(QCString&);
    protected slots:
        void slotPushOut(Opie::Core::OProcess*, char*, int);
        void slotPushErr(Opie::Core::OProcess*, char*, int);
        void slotPushExited(Opie::Core::OProcess* proc);
    protected:
        /**
         * Function makes a notification from slotPushOut and slotPushErr
         */
        void notifyInfo(Opie::Core::OProcess*, char*, int);
    protected:
        Opie::Core::OProcess* pushProc; //The push process
    };
};
#endif
//eof
