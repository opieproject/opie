/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef SECURITY_H
#define SECURITY_H

#include "securitybase.h"

class QPEDialogListener;

class Security : public SecurityBase
{ 
    Q_OBJECT

public:
    Security( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Security();

    void show();

protected:
    void accept();
    void applySecurity();
    void done(int);

private slots:
    void changePassCode();
    void clearPassCode();
    void setSyncNet(const QString&);

private:
    bool telnetAvailable() const;
    bool sshAvailable() const;
    void updateGUI();

    static void parseNet(const QString& sn,int& auth_peer,int& auth_peer_bits);
    void selectNet(int auth_peer,int auth_peer_bits);
    
    QString enterPassCode(const QString&);
    QString passcode;
    bool valid;

    QPEDialogListener *dl;
};


#endif // SECURITY_H

