/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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

#ifndef _ADDRESSSETTINGS_H_
#define _ADDRESSSETTINGS_H_

#include <qlist.h>
#include <qstringlist.h>
#include "addresssettingsbase.h"

const int ADDRESSVERSION = 3;

class AddressSettings : public AddressSettingsBase
{
    Q_OBJECT
public:
    AddressSettings( QWidget *parent = 0, const char *name = 0 );
    ~AddressSettings();

protected:
    void accept();
    virtual void itemUp();
    virtual void itemDown();

private:
    void init();
    void save();
};

#endif  // _ADDRESSSETTINGS_H_
