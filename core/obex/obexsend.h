#ifndef OPIE_OBEX_SEND_WIDGET_H
#define OPIE_OBEX_SEND_WIDGET_H

//  7-Jul-2005 mbh@sdgsystems.com: replace hand coded form with one
//             generated via QT2 Designer.  The new form supports
//             selection of target devices, as opposed to sending to
//             all.

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include "obexsendbase.h"

class QLabel;
class QVBoxLayout;
/**
 * This is the new sending widget for Obex
 * It will attemp to smart and be able to send
 * it to multiple (selected) devices.
 * It'll support BT + IrDa
 */
namespace OpieObex {
    class Obex;
#ifdef BLUETOOTH
    class BtObex;
#endif

    struct Pair {
        Pair(const QString& first = QString::null,
             const QString& second = QString::null)
            : m_first(first), m_second(second ) {
        }
        QString first()const{ return m_first; }
        QString second()const { return m_second; }
    private:
        QString m_first;
        QString m_second;
    };
    class SendWidget : public obexSendBase {
        Q_OBJECT
    public:
        SendWidget( QWidget* parent = 0, const char* name = 0);
        ~SendWidget();

        QString file()const;

    protected:
        void closeEvent( QCloseEvent* );
#ifdef BLUETOOTH
	void read_receivers();
#endif

    public slots:
        void send( const QString& file, const QString& desc );

    signals:
        void done();

    protected slots:
        virtual void userDone();
        virtual void send_to_receivers();
        virtual void scan_for_receivers();
	virtual void toggle_receiver(QListViewItem* item);

    private slots: // QCOP slots
        /* IrDa Names*/
        void slotIrDaDevices( const QStringList& );
        /* Bt Names + BD-Addr */
        void slotBTDevices( const QMap<QString, QString>& );

        void slotSelectedDevice( int id, int dev );

        void dispatchIrda( const QCString& str, const QByteArray& ar );

        void slotIrError( int );
        void slotIrSent(bool);
        void slotIrTry(unsigned int );
        void slotStartIrda();

        void dispatchBt( const QCString& str, const QByteArray& ar );
        void slotBtError( int );
        void slotBtSent(bool);
        void slotBtTry(unsigned int );
        void slotStartBt();

    private:
        void initUI();
	int addReceiver(const QString& str, const char *icon);
	void setReceiverStatus( int id, const QString& status );
	bool receiverSelected(int id);

        int m_start;
        QMap<int, QString> m_irDa;
        QMap<int, QString>::Iterator m_irDaIt;
#ifdef BLUETOOTH
        QMap<int, Pair > m_bt;
        QMap<int, Pair>::Iterator m_btIt;
#endif
	QMap<int, QListViewItem *> receivers;
        QString m_file;
        Obex* m_obex;
#ifdef BLUETOOTH
        BtObex* m_btobex;
#endif
    };
}

#endif
