#ifndef OPIE_FILE_RECEIVE_H
#define OPIE_FILE_RECEIVE_H

/**
 * This is the receive Implementation
 * for X-Modem/Y-Modem/Z-Modem
 */
#include <sys/types.h>

#include "receive_layer.h"

class QSocketNotifier;
class FileReceive : public ReceiveLayer {
    Q_OBJECT
public:
    enum Type {
        SZ = 0,
        SX,
        SY
    };
    FileReceive( Type t, IOLayer* lay, const QString& startDir = QString::null );
    ~FileReceive();
    void receive();
    void receive( const QString& dir );
    void cancel();
private slots:
    void setupChild();
    void slotRead();
    void slotExec();
private:
    pid_t m_pid;
    int m_fd;
    int m_prog;
    int m_info[2];
    int m_comm[2];
    int m_term[2];
    Type m_type;
    QSocketNotifier* m_not;
    QSocketNotifier* m_proc;
};

#endif
