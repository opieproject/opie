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
        void slotSelectedDevice( const QString& name, int dev );
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
        QStringList m_irDa;
        QMap<QString, QString> m_bt;
        QString m_file;
        Obex* m_obex;
    };
    class DeviceItem  {
    public:
        DeviceItem( const QString& name = QString::null,
                    const QString& status = QString::null, int dev = 3);
        ~DeviceItem();
        void setStatus( const QString& text );

        QString name()const;
        QString status()const;
        QString pixmap()const;
        int device()const;
        QString toString()const;
    private:
        QString m_name;
        QString m_status;
        int m_dev;
    };
    class DeviceBox : public QTextBrowser {
        Q_OBJECT
    public:
        enum Device { IrDa, BT, Search, Error };
        DeviceBox( QWidget* parent );
        ~DeviceBox();

        void setSource( const QString& str );
        void addDevice( const QString& name, int dev,
                        const QString& status = QString::null );
        void removeDevice( const QString& name );
        void setStatus( const QString& name, const QString& );
    signals:
        void selectedDevice( const QString& name, int dev );
    private:
        QMap<QString, DeviceItem> m_dev;
        QStringList m_devices;

    };
}

#endif
