#ifndef OPIE_OBEX_SEND_WIDGET_H
#define OPIE_OBEX_SEND_WIDGET_H

#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qmap.h>
#include <qtextbrowser.h>

class QLabel;
class QVBoxLayout;
/**
 * This is the new sending widget for Obex
 * It will attemp to smart and be able to send
 * it to multiple devices.
 * It'll support BT + IrDa
 */
namespace OpieObex {
    class DeviceBox;
    class Obex;

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
    class SendWidget : public QWidget{
        Q_OBJECT
    public:
        SendWidget( QWidget* parent = 0, const char* name = 0);
        ~SendWidget();

        QString file()const;

    public slots:
        void send( const QString& file, const QString& desc );

    signals:
        void done();

    private slots: // QCOP slots
        /* IrDa Names*/
        void slotIrDaDevices( const QStringList& );
        /* Bt Names + BD-Addr */
        void slotBTDevices( const QMap<QString, QString>& );
        void slotSelectedDevice( int id, int dev );
        void dispatchIrda( const QCString& str, const QByteArray& ar );
        void dispatchBt( const QCString& str, const QByteArray& ar );

        void slotIrError( int );
        void slotIrSent(bool);
        void slotIrTry(unsigned int );
        void slotStartIrda();
        void slotDone();
    private:
        void initUI();
        QLabel* m_lblFile;
        DeviceBox* m_devBox;
        QVBoxLayout* m_lay;
        int m_start;
        QMap<int, QString> m_irDa;
        QMap<int, QString>::Iterator m_irDaIt;
        QMap<int, Pair > m_bt;
        QString m_file;
        Obex* m_obex;
        int m_irDeSearch; // search of irda and bt devices
        int m_btDeSearch;
    };
    class DeviceItem  {
    public:
        DeviceItem( const QString& name = QString::null,
                    const QString& status = QString::null, int dev = 3,  int id = -1);
        ~DeviceItem();
        void setStatus( const QString& text );

        QString name()const;
        QString status()const;
        QString pixmap()const;
        int device()const;
        int id()const;
        QString toString()const;
    private:
        QString m_name;
        QString m_status;
        int m_dev;
        int m_id;
    };

    /*
     * The text field which contains the information about sending...
     *
     */
    class DeviceBox : public QTextBrowser {
        Q_OBJECT
    public:
        enum Device { IrDa, BT, Search, Error };
        DeviceBox( QWidget* parent );
        ~DeviceBox();

        void setSource( const QString& str );
        int addDevice( const QString& name, int dev,
                       const QString& status = QString::null );
        void removeDevice( int );
        void setStatus( int, const QString& );
    signals:
        void selectedDevice( int id, int dev );
    private:
        /* returns a id for a device from a device range */
        int idFor (int deviceType );
        QString allText();
        QMap<int, DeviceItem> m_dev;
    };
}

#endif
