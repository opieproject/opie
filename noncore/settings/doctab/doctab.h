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
#ifndef DOCTAB_H
#define DOCTAB_H


#include <qstrlist.h> 
#include <qasciidict.h>
#include "doctabsettingsbase.h"

class QPEDialogListener;

class DocTabSettings : public DocTabSettingsBase
{ 
    Q_OBJECT

public:
    DocTabSettings( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DocTabSettings();

    static QString appName() { return QString::fromLatin1("doctab"); }

protected:
    void accept();
    void reject();
    void done(int);

private slots:
    void applyDocTab();
    void reset();

private:
    static QString actualDocTab;

    QPEDialogListener *dl;
};


#endif // SETTINGS_H

