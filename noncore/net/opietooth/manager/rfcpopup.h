#ifndef RFCPOPUP_H
#define RFCPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>

namespace OpieTooth {
    /**
     * A simple reference implementation for
     * the popup helper factory.
     * This class derives from QPopupMenu and uses
     * plugged QActions to do all the nasty in it's
     * slots. After the work is done everything must
     * be deleted.
     */
    class RfcCommPopup : public QPopupMenu {
        Q_OBJECT
    public:
        RfcCommPopup();
        ~RfcCommPopup();


    private:
        QAction* m_con;
        QAction* m_dis;
        QAction* m_foo;
        QAction* m_bar;
    private slots:
        void slotConnect();
        void slotDisconnect();
        void slotFoo();
        void slotBar();
    };
};

#endif
