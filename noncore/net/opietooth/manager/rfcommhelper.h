
#ifndef OPIETOOTH_RFCOMM_HELPER_H
#define OPIETOOTH_RFCOMM_HELPER_H

#include <sys/types.h>

#include <qcstring.h>
#include <qstring.h>

namespace OpieTooth {
    /**
     * RfCommHelper helps to attach to a remote
     * channel of a bt device.
     * It uses the commandline tool rfcomm fo the job
     * it tries to attach to afree node and either succeeds
     * or fails.
     * Later RfCommHelper can be used to detach the device
     * from the remote device
     */
    class RfCommHelper {
    public:
        /**
         * c'tor
         */
        RfCommHelper();

        /**
         * d'tor removes the device
         */
        ~RfCommHelper(); // kills the process

        /**
         * gives the device name of the attached device
         */
        QCString attachedDevice() const;

        /**
         * detach a device
         * kills the rfcomm process
         */
        void detach();

        /**
         * tries to connect to a remote device
         * @param bd_addr the bluetooth address
         * @param port The port of the remote device
         */
        bool attach(const QString& bd_addr, int port );

        /**
         * If the original user of the bond exists
         * it may regroup the rfcomm process
         */
        void regroup();
    private:
        bool connect( int dev, const QString& bdaddr, int port );
        static void signal_handler(int);
        void setupComChild();
        bool m_connected:1;
        static pid_t m_pid;
        QCString m_device;
        int m_fd[2];
        int m_in2out[2];
        int m_out2in[2];
        static bool terminate ;
    };
};

#endif
