#ifndef PANPOPUP_H
#define PANPOPUP_H

#include <qpopupmenu.h>
#include <qaction.h>
#include <opie/oprocess.h>

#include "btdeviceitem.h"

namespace OpieTooth {

    class PanPopup : public QPopupMenu {

	Q_OBJECT

    public:
        PanPopup( OpieTooth::BTDeviceItem* );
        ~PanPopup();

    private:
        QAction* m_push;
	OProcess* m_panconnect;
	OpieTooth::BTDeviceItem *m_item;
    private slots:
        void slotConnect();
        void slotConnectAndConfig();
	void slotExited( OProcess* proc );
	void slotStdOut( OProcess* proc, char* chars, int len );
    };
};

#endif
