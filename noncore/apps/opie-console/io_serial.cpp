
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include "io_serial.h"

IOSerial::IOSerial(const Profile &config) : IOLayer(config) {
    m_read = 0l;
    m_error = 0l;
    m_fd = 0;
    m_connected = false;
    reload(config);
}


IOSerial::~IOSerial() {
    if (m_fd) {
        close();
    }
}

void IOSerial::send(const QByteArray &data) {
    if (m_fd) {
        write(m_fd, data.data(), data.size());
    } else {
        emit error(Refuse, tr("Not connected"));
    }
}

void IOSerial::close() {
    if (m_fd) {
        delete m_read;
        delete m_error;
        ::close(m_fd);
        m_fd = 0;
        m_connected = false;
    } else {
        m_connected = false;
        emit error(Refuse, tr("Not connected"));
    }
}

bool IOSerial::open() {
    if (!m_fd) {
        struct termios tty;
        m_fd = ::open(m_device, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (m_fd < 0) {
            emit error(CouldNotOpen, strerror(errno));
            m_fd = 0;
            return FALSE;
        }
        tcgetattr(m_fd, &tty);

        /* Baud rate */
        int speed = baud(m_baud);
        if (speed == -1) {
            emit error(Refuse, tr("Invalid baud rate"));
        }
        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        /* Take care of Space / Mark parity */
        if (m_dbits == 7 && (m_parity == ParitySpace || m_parity == ParityMark)) {
            m_dbits = 8;
        }

        /* Data bits */
        switch (m_dbits) {
            case 5: tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5; break;
            case 6: tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6; break;
            case 7: tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7; break;
            case 8: tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; break;
	    default: break;
        }

        /* Raw, no echo mode */
        tty.c_iflag =  IGNBRK;
        tty.c_lflag = 0;
        tty.c_oflag = 0;
        tty.c_cflag |= CLOCAL | CREAD;

        /* Stop bits */
        if (m_sbits == 2) {
            tty.c_cflag |= CSTOPB;
        } else {
            tty.c_cflag &= ~CSTOPB;
        }

        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 5;

        /* Flow control */
        if (m_flow & FlowSW)
            tty.c_iflag |= IXON | IXOFF;
        else
            tty.c_iflag &= ~(IXON|IXOFF|IXANY);

        if (m_flow & FlowHW)
            tty.c_cflag |= CRTSCTS;
        else
            tty.c_cflag &= ~CRTSCTS;

        /* Parity */
        tty.c_cflag &= ~(PARENB | PARODD);
        if (m_parity & ParityEven)
            tty.c_cflag |= PARENB;
        else if (m_parity & ParityOdd)
            tty.c_cflag |= (PARENB | PARODD);

        /* Set the changes */
        tcsetattr(m_fd, TCSANOW, &tty);

        /* Notifications on read & errors */
        m_read = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
        m_error = new QSocketNotifier(m_fd, QSocketNotifier::Exception, this);
        connect(m_read, SIGNAL(activated(int)), this, SLOT(dataArrived()));
        connect(m_error, SIGNAL(activated(int)), this, SLOT(errorOccured()));
        m_connected = false;
        return TRUE;
    } else {
        emit error(Refuse, tr("Device is already connected"));
        m_fd = 0;
        return FALSE;
    }
}

void IOSerial::reload(const Profile &config) {
    m_device = config.readEntry("Device", SERIAL_DEFAULT_DEVICE);
    m_baud = config.readNumEntry("Speed", SERIAL_DEFAULT_BAUD);
    m_parity = config.readNumEntry("Parity", SERIAL_DEFAULT_PARITY);
    m_dbits = config.readNumEntry("DataBits", SERIAL_DEFAULT_DBITS);
    m_sbits = config.readNumEntry("StopBits", SERIAL_DEFAULT_SBITS);
    m_flow = config.readNumEntry("Flow", SERIAL_DEFAULT_FLOW);

}

int IOSerial::baud(int baud) const {
    switch (baud) {
        case 300:    return B300;    break;
        case 600:    return B600;    break;
        case 1200:   return B1200;   break;
        case 2400:   return B2400;   break;
        case 4800:   return B4800;   break;
        case 9600:   return B9600;   break;
        case 19200:  return B19200;  break;
        case 38400:  return B38400;  break;
        case 57600:  return B57600;  break;
        case 115200: return B115200; break;
    }
    return -1;
}

void IOSerial::errorOccured() {
    emit error(ClosedUnexpected, strerror(errno));
    close();
}

void IOSerial::dataArrived() {
    QByteArray array(4097);

    int len = read(m_fd, array.data(), 4096);
    if (len == 0)
        close();
    if (len < 0)
        return;
    qWarning("got from layer");
    for (int i = 0; i < len; i++ ) {
        printf("%c", array[i] );
    }
    printf("\n");
    array.resize( len );
    emit received(array);
}

QString IOSerial::identifier() const {
    return "serial";
}

QString IOSerial::name() const {
    return "RS232 Serial IO Layer";
}
int IOSerial::rawIO()const {
    if (m_read )
        disconnect(m_read, SIGNAL(activated(int)), this, SLOT(dataArrived()));
    if (m_error )
        disconnect(m_error, SIGNAL(activated(int)), this, SLOT(errorOccured()));

    int fd = ::open(m_device, O_RDWR  );

    return fd;
};
void IOSerial::closeRawIO(int fd) {
    if (m_read )
        connect(m_read, SIGNAL(activated(int)), this, SLOT(dataArrived()));
    if (m_error )
        connect(m_error, SIGNAL(activated(int)), this, SLOT(errorOccured()));

    ::close( fd );
}
QBitArray IOSerial::supports()const {
    QBitArray ar(3);
    ar[0] = ar[2] = 0;
    ar[1] = 1;

    return ar;
}

bool IOSerial::isConnected() {
    return m_connected;
}

/*
 * this is used to give the
 * class below IOSerial
 * the possibility of
 * exclusive usage
 */
void IOSerial::internDetach() {
    if (m_read )
        disconnect(m_read, SIGNAL(activated(int)), this, SLOT(dataArrived()));
    if (m_error )
        disconnect(m_error, SIGNAL(activated(int)), this, SLOT(errorOccured()));
}
/*
 * give power back
 */
void IOSerial::internAttach() {
    if (m_read )
        connect(m_read, SIGNAL(activated(int)), this, SLOT(dataArrived()));
    if (m_error )
        connect(m_error, SIGNAL(activated(int)), this, SLOT(errorOccured()));
}
