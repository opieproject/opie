#ifndef OPIE_RECEIVE_LAYER_H
#define OPIE_RECEIVE_LAYER_H

#include <qobject.h>

/**
 * An abstract Layer for receiving files
 * from an IOLayer connection
 * It gives us the possibility to listen
 * to get progress to stop listen
 * to get errors
 */
class IOLayer;
class ReceiveLayer : public QObject {
    Q_OBJECT
public:
    /**
     * How to receive files
     * How handhel \r\n?
     * ASCII or binary
     */
    enum Mode {
        Ascii = 0l,
        Binary
    };

    /**
     * What features to use
     * DISCUSS IT!!!!
     * see rz --help for more info
     */
    enum Features {
        Append = 0,
        AllowRemoteCommands = 1,
        WriteToNull = 2,
        Escape = 4,
        Rename = 8,
        OpenSync = 16,
        ProtectExisting = 32,
        Resume = 64,
        KeepUppercase = 128,
        DisableRestrict = 256,
        Restricted = 512,
        Overwrite = 1024
    };

    /**
     * Error codes
     */
    enum Error {
        Unknown = 0,
        StartError
    };

    /**
     * C'tor constructs an new Object
     * @param lay The Layer to be used
     * @param t The Type
     * @param startDir In which dir should files be received?
     */
    ReceiveLayer( IOLayer* lay, const QString& startDir = QString::null );
    virtual ~ReceiveLayer();

public slots:
    /**
     * start receiving in current dir
     * with protocol from the c'tor
     */
    virtual void receive() = 0;

    /**
     * start  to receive in dir with type
     * from the c'tor
     */
    virtual void receive( const QString& dir ) = 0;

    /**
     *  advanced method with features and Mode
     */
    virtual void receive( const QString& dir,  Mode, Features );

    /**
     * cancel receive
     */
    virtual void cancel();

signals:
    /**
     * error happend
     * error code as int
     * and a QString for UI translated string
     */
    void error(int, const QString& );

    /**
     * progress
     * @param file The completed path to the file which is received
     * @param speed the speed in bps
     * @param hour The hours remaining
     * @param minutes The miniutes remaining
     * @param seconds The seconds remaining
     */
    void progress( const QString& file, int progress, int speed, int hour, int min, int seconds );

    /**
     * completely received a file
     */
    void received( const QString& file );

protected:
    QString m_curDir;
    IOLayer* layer();
    /* from a variable set from the outside */
    QString currentDir()const;
    void changeDir( const QString& );
private:
    IOLayer* m_layer;

};

#endif
