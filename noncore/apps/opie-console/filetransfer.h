#ifndef OPIE_FILE_TRANSFER_H
#define OPIE_FILE_TRANSFER_H

#include <sys/types.h>

#include <qfile.h>

#include "file_layer.h"

class QSocketNotifier;
class OProcess;
class FileTransfer : public FileTransferLayer{
    Q_OBJECT
public:
    enum Type {
        SZ = 0,
        SX,
        SY
    };
    FileTransfer( Type t, IOLayer* );
    ~FileTransfer();

    void sendFile( const QString& file );
    void sendFile( const QFile& );

private slots:
    void setupChild();
private:
    static pid_t m_pid;
    int m_fd;
    int m_info[2];
    int m_comm[2];
    Type m_type;
    static void signal_handler(int);
    static bool terminate;
};

#endif
