#ifndef OBEXPOPUP_H
#define OBEXPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>

namespace OpieTooth {

    class ObexPopup : public QPopupMenu {
        Q_OBJECT
    public:
        ObexPopup();
        ~ObexPopup();


    private:
        QAction* m_push;
    private slots:
        void slotPush();
    };
};

#endif
