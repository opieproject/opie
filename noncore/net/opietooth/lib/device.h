
#ifndef OpieToothDevice_H
#define OpieToothDevice_H

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <sys/types.h>

namespace Opie {namespace Core {class OProcess;}}
namespace OpieTooth {
    /**
     *  Device takes care of attaching serial
     *  devices to the blueZ stack.
     *  After attaching it hciconfig ups it
     */
    class Device : public QObject {

        Q_OBJECT

    public:

    /**
     * Brings up an device.
     * Usage example: new Device(/dev/ttySB0, csr)
     *
     * @param &device QString the device name
     * @param &mode QString the mode
     * @param &speed QString the speed of the device, can be left blank
     */
    Device(const QString &device, const QString& mode, const QString& speed);

        /**
         * unloads the device
         */
        ~Device();

        /**
         * attach the device
         */
        void attach();

        /**
         * detach the device
         */
        void detach();

        /**
         * Is the device loaded?
         * @return bool, if the device is loaded
	 */
        bool isLoaded()const;

        /**
         * Returns the device name
         * @return QString, the device name
         */
        QString devName()const ; // hci0

    signals:

        /**
         * Signals devicename and up status
         * @return &device QString, Devicename
         * @return up bool, if the device is up or not.
         */
        void device(const QString& device, bool up );
    private slots:
        virtual void slotExited( Opie::Core::OProcess* );
        virtual void slotStdOut(Opie::Core::OProcess*, char*, int );
	virtual void slotStdErr(Opie::Core::OProcess*, char*, int );
    private:
	class Private;
	Private *d;
	QString m_device;
	bool m_attached:1;
	Opie::Core::OProcess* m_hci;
	Opie::Core::OProcess* m_process;
	QString m_devId;
	QString m_mode;
        QString m_speed;
	pid_t pid;
	QCString m_output;
    };
};

#endif
