#ifndef OTDEVICE_H
#define OTDEVICE_H

#include <qobject.h>
#include <qstring.h>

#include <bluezlib.h>

#include <OTGateway.h>

// define if you want to use the process hciattach for detection
// #define USEHCIPROC

class QTimerEvent;

namespace Opie {
    namespace Core {
       class OProcess;
    }
}

namespace Opietooth2 {

    /**
     *  Device takes care of attaching serial
     *  devices to the blueZ stack.
     *  After attaching it hciconfig ups it
     */
    class OTDevice : public QObject {

        Q_OBJECT

    public:

        /**
         * Brings up the device.
         * will detech which device is needed
         * Usage example: new Device(/dev/ttySB0, csr)
         *
         */
        OTDevice( OTGateway * _OT );

        /**
         * unloads the device
         */
        ~OTDevice();

        inline  OTGateway * gateway() const
          { return OT; }

        inline bool needsAttach( void ) const
          { return NeedsAttach; }

        inline int deviceNr() const
          { return m_deviceNr; }

        /**
         * attach the device
         * @return bool which is TRUE if command started sucessfully
         */
        bool attach();

        /**
         * detach the device
         * @return bool which is TRUE if command started sucessfully
         */
        bool detach();

        /**
         * Is the device loaded?
         * @return bool, if the device is loaded
	 */
        bool isAttached()const;

        // check current state of attachment
        bool checkAttach();

        // returns pattern that points to the proper
        // RFCOMM device file.  Put %1 where device nr should be put
        // e.g. : /dev/bluetooth/rfcomm/%1 or /def/rfcomm%1
        QString getRFCommDevicePattern();

    signals:

        /**
         * Signals if the attach/detach of the device produced
         * errors
         * @param Mesg contains an error message
         */
        void error( const QString & mesg );
        void isEnabled( int devnr, bool );

    private slots:

        void slotStdOut(Opie::Core::OProcess*, char*, int );
	void slotStdErr(Opie::Core::OProcess*, char*, int );

    private:

	void detectDeviceType( QString & Device,
                               QString & Mode,
                               unsigned long & Speed );
	pid_t getPidOfHCIAttach( void );

	Opie::Core::OProcess* m_hciattach; // ptr to hciattach proces

        int           m_deviceNr; // x as in hci(x)
	pid_t         m_hciattachPid; // pid of hciattach program

	// backpointer
	OTGateway *   OT;
	bool          NeedsAttach;
    };
}

#endif
