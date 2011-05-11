/* $Id: rfcommassigndialogimpl.h,v 1.3 2006-03-25 18:10:13 korovkin Exp $ */
/* RFCOMM binding table edit dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef RFCOMMASSIGN
#define RFCOMMASSIGN

#include "rfcommassigndialogbase.h"
#include "rfcommassigndialogitem.h"
#include "rfcommconfhandler.h"

#include <qscrollview.h>
#include <qmap.h>
#include <qvbox.h>

namespace OpieTooth {

    class RfcommAssignDialog: public RfcommAssignDialogBase {
        Q_OBJECT
    public:
        RfcommAssignDialog(QWidget* parent = 0, const char* name = 0, bool modal = 0, WFlags fl = 0);
        ~RfcommAssignDialog();

        void loadConfig();
        void saveConfig();

        void newDevice( const QString & mac, int channel );

    private:
        QScrollView *m_scrollView;
        // how many rfcomm devices are possible
        int m_range;
        QMap< int, RfcommDialogItem* > m_itemList;
        QVBox *m_box;
        RfCommConfHandler *confHandler;
    protected slots:
        virtual void accept();
};

}

#endif
