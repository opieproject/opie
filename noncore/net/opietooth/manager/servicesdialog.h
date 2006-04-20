/* $Id: servicesdialog.h,v 1.1 2006-04-20 12:37:33 korovkin Exp $ */
/* Bluetooth services configuration dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _SERVICESDIALOG_H_
#define _SERVICESDIALOG_H_
#include "servicesdialogbase.h"
#include "btconfhandler.h"
namespace OpieTooth {
    class ServicesDialog : public ServicesDialogBase {
        Q_OBJECT
    public:
        ServicesDialog(const QString& conf,
            QWidget* parent = 0, const char* name = 0, bool modal = TRUE, 
            WFlags fl = 0);
        ~ServicesDialog();
    public:
        BTConfHandler cfg; //configuration file
    protected slots:
        virtual void accept();
    };
};
#endif
//eof
