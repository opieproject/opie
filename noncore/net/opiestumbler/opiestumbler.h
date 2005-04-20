#ifndef OPIESTUMBLER_H
#define OPIESTUMBLER_H

#include <qmainwindow.h>
#include <qlist.h>

#include "stumblerstation.h"

class QString;
class QPopupMenu;
class Stumbler;
class QCopChannel;
class QListView;
class QListViewItem;

namespace Opie{
    namespace Net {
        class OMacAddress;
        class OStation;
        class OManufacturerDB;
    }

    namespace Core {
        class OProcess;
    }
}
            

class OpieStumbler: public QMainWindow {
    Q_OBJECT
public:

    enum CurrentColumns { CURSSID, CURCHAN, CURSIGNAL, CURENC };
    enum HistoryColumns { HISSSID, HISCHAN, HISSIGNAL, HISENC, HISVENDOR };
    OpieStumbler(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    static QString appName() { return QString::fromLatin1("opiestumbler"); }
    static QString appCaption();
    void displayStations();
    QString manufacturer(const QString &mac, bool extended = FALSE );
protected slots:
    void slotConfigure();
    void slotStartScanning();
    void slotStopScanning();
    void slotUpdateStations();
    void slotMessageReceived( const QCString &, const QByteArray & );
    void slotCurrentMousePressed(int button, QListViewItem *item, const QPoint &point, int c);
    void slotHistoryMousePressed(int button, QListViewItem *item, const QPoint &point, int c);
    void slotShowDetails();
    void slotLoadManufacturers();
    void slotJoinNetwork();
    void slotAssociated();
    void slotCheckDHCP();
protected:
    void loadConfig();
    QListView *m_listCurrent;
    QListView *m_listHistory;
    QString m_interface;
    Stumbler *m_stumbler;
    QCopChannel *m_channel;
    QList <Opie::Net::OStation> *m_stationsCurrent;
    QList <StumblerStation> m_stationsHistory;
    QPopupMenu *m_popupCurrent;
    QPopupMenu *m_popupHistory;
    Opie::Net::OManufacturerDB *m_db;
    QString m_mac;
    Opie::Core::OProcess *m_proc;
    
};

#endif /* OPIESTUMBLER_H */
