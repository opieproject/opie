#ifndef OPIE_OBEX_RECEIVER_H
#define OPIE_OBEX_RECEIVER_H

#include <qobject.h>
#include <qvbox.h>
#include <qstring.h>

class QLabel;
class QTextView;
namespace OpieObex {
    class Obex;
    class OtherHandler;
    class Receiver : public QObject {
        Q_OBJECT
    public:
        enum { Datebook , AddressBook, Other };
        Receiver();
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
        void tidyUp( QString& file );

    private slots:
        void slotReceived( const QString& );

    private:
        Obex* m_obex;
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
