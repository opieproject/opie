#ifndef OPIE_FILE_TRANSFER_H
#define OPIE_FILE_TRANSFER_H

#include <sys/types.h>

#include <qfile.h>
#include <qstringlist.h>

#include "file_layer.h"

class QSocketNotifier;
class OProcess;
class FileTransferControl;
class FileTransfer : public FileTransferLayer{
    Q_OBJECT
    friend class FileTransferControl;
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
    void cancel();

private slots:
    void setupChild();
    void slotRead();
    void slotProgress( const QStringList& );
private:
    /*
     * FIXME? What does happen if we've
     * two FileTransfers at a time?
     * Have a procctl which does listen
     * for termination and then send a signal
     */
    static pid_t m_pid;
    int m_fd;
    int m_prog;
    int m_info[2];
    int m_comm[2];
    QString m_file;
    Type m_type;
    QSocketNotifier *m_not;
    static void signal_handler(int);
    static bool terminate;
};

#endif
