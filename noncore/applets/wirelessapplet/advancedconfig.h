/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer
**               <mickey@tm.informatik.uni-frankfurt.de>
**               http://www.Vanille.de
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef ADVANCED_CONFIG_H
#define ADVANCED_CONFIG_H

#include "advancedconfigbase.h"

#include <qnamespace.h>

class AdvancedConfig: public AdvancedConfigBase
{
    Q_OBJECT
    
public:
    AdvancedConfig( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AdvancedConfig();

protected:
    virtual void accept();

};

#endif
