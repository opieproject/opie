//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OTINQUIRY_H
#define OTINQUIRY_H

#include <qstring.h>
#include <qobject.h>

#include <qguardedptr.h>
#include <qtimer.h>

#include <OTPeer.h>

// #include <set>
// #include <deque>

namespace Opietooth2 {

class QSocket;
class QDateTime;

class OTDriver;
class OTHCISocket;

/** Scans for nearby bluetooth devices.
 * This class provides an asynchronous interface to the
 * inquriy HCI command. To scan for other devices, connect
 * to the signals neighbourFound() and finished() and call
 * inquiry(). Inquiry results are signalled as soon as they arrive,
 * so the information can be displayed before the whole inquiry
 * process is finished.
 * Still no connections should be set up before
 * the finished() signal was sent (hmm, is this always true..?)
 */

class OTInquiry : public QObject {

    Q_OBJECT

public:

    /** Constructor.
    @param owner The owning parent object
    */
    OTInquiry( OTDriver* Drv );

    /** Destructor. */
    virtual ~OTInquiry();

    // General/Unlimited Inquiry Access Code
    static const int GIAC = 0x9E8B33;

    // Limited Dedicated Inquiry Access Code
    static const int LIAC = 0x9E8B00;

    /** Starts the inquiry.
    if you start inquiry several times without calling clear(),
    you will receive each result only once.
    @param timeout duration of the inquiry in seconds.
    It will be rounded to the next multiple of 1.28 sec,
    with a maximum of 61.44 sec.
    @param lap
    */
    bool inquire( double timeout = 8.0, 
                  int numResponses = 0,
                  int lap = GIAC);

    void stopInquiring( );

    /** Enters periodic inquiry mode.
    An inquiry will be started at a random time in the intervall
    between minduration and maxduration.
    @param minduration minimum time between two inquiries. Rounded to
        a multiple of 1.28, (3.84 <= d <= 83884.8)
    @param maxduration maximum time between two inquiries. Rounded to
        a multiple of 1.28, (3.84 <= d <= 83884.8)
    @param timeout duration of the inquiry in seconds.
        It will be rounded to the next multiple of 1.28 sec,
        with a maximum of 61.44 sec.
    @param numResponses Number of responses after which the inquiry
        will be stopped. 0 means no limit.
    @param lap
    */
    /*
    void inquirePeriodically( double minduration, 
                              double maxduration,
                              double timeout = 8.0, 
                              int numResponses = 0,
                              int lap = LIAC
                            );
*/
    /** checks if there the inquiry is running currently
    @return true if there is an inquiry running
    which was started by this object.
    @todo possibility to check for "foreign" inquiries. Deal with
    the fact that we can receive foreign inquiry results.
    */
    bool isInquiring();

    /** This function returns true after after an inquiry was
    started, results were received and the inquiry ended successfully.
    This can be the case without calling inquiry() at all, because
    results of inquiries started by other programs are also processed. 
    */
    bool isFinished();
      
    /** Resets the list of received results and sets 
    isInquiryComplete() to false.
    */
    void reset();

    inline OTDriver * driver() const
      { return Driver; }
      
protected:

    /** Called when a new neighbour was found. The default
    implementation emits the neighbourFound signal.
    @param bdaddr the address found.
    */
    virtual void onPeerFound( OTPeer * Peer, bool  );

    enum ErrorCode {
         InquiryFinishedTimeout = 0x0100
    };

signals :

    void peerFound( OTPeer *, bool );

    /** Emitted after an inquiry has finished successfully.
    If the inquiry was canceled, no finished signals is emitted.
    This signal can be emitted without having called inquiry, since
    other processes may have started an inquiry.
    */
    void finished();

    /** Emitted instead of finished if an error occured after
    calling inquiry() or periodicInquiryMode()
    @param code error code.
    @param message Error message
    */
    void error( QString message );

private:

    // std::set<DeviceAddress> addrCache;
    // double currentTimeout;
    // QByteArray* buf;
    // QSocket* hciSocket;
    QGuardedPtr<OTHCISocket> Socket;
    OTDriver * Driver;

    //QDateTime *startTime;
    QTimer *InquiryTimeoutTimer;

    // std::deque<InquiryInfo> infoQueue;
    bool SuccessfullyStarted;
    bool SuccessfullyEnded;
    
private slots:

    void slotInquiryTimeout();
    void slotHCIEvent(unsigned char eventCode, QByteArray buf);

};

};
#endif
