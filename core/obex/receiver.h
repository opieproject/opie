#ifndef OPIE_OBEX_RECEIVER_H
#define OPIE_OBEX_RECEIVER_H

#include <qobject.h>
#include <qvbox.h>
#include <qstring.h>

//Receiver type
typedef enum _RecType {
    REC_IRDA = 0,
    REC_BLUETOOTH = 1
} RecType;

class QLabel;
class QTextView;
namespace OpieObex {
    class ObexBase;
    class OtherHandler;
    class Receiver : public QObject {
        Q_OBJECT
    public:
        enum { Datebook , AddressBook, Other };
        Receiver(RecType type);
        ~Receiver();

    private:
        void handleAddr(const QString& );
        void handleDateTodo(const QString& );
        void handleOther(const QString& );
        /* will alter the file name */
        int checkFile( QString& file );
        bool testDateTodo(const QString& file);
        bool testAddressbook(const QString& file);
        /* called by checkFile */
        void tidyUp( QString& file, const QString& ending );

    private slots:
        void slotReceived( const QString& );

    private:
        ObexBase* m_obex; //IR obex
    };

    class OtherHandler : public QVBox {
        Q_OBJECT
    public:
        OtherHandler();
        ~OtherHandler();

        void handle( const QString& file );
    private slots:
        void accept();
        void deny();
    private:
        QString targetName( const QString& file );
        void copy( const QString& src, const QString& dest );
        QLabel* m_na;
        QTextView* m_view;
        QString m_file;
    };
}


#endif
