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

#include <qvaluelist.h>
#include <qpe/qpeapplication.h>
#include <qdir.h>
#include <qwidgetstack.h>

#include "calc.h"
#include "plugininterface.h"

calc::calc (QWidget * p = 0, const char *n = 0):QWidget (p, n)
{
  setCaption (tr ("Calculator"));

// widgets
  LCD = new QLCDNumber (this);
  LCD->setMaximumSize (QSize (240, 30));
    LCD->setNumDigits(12);
LCD->setSegmentStyle(QLCDNumber::Filled);
  pluginWidgetStack = new QWidgetStack (this);

// layout widgets
  calculatorLayout = new QVBoxLayout (this);
  calculatorLayout->addWidget (LCD);
  calculatorLayout->addWidget (pluginWidgetStack);

// no formatting of display for now
  connect (&engine, SIGNAL(display (double)), LCD, SLOT(display (double)));
  connect (&engine, SIGNAL(display (const QString &)), LCD, SLOT(display (const QString &)));
    connect (&engine, SIGNAL(setBinMode()), LCD, SLOT(setBinMode()));
    connect (&engine, SIGNAL(setOctMode()), LCD, SLOT(setOctMode()));
    connect (&engine, SIGNAL(setDecMode()), LCD, SLOT(setDecMode()));
    connect (&engine, SIGNAL(setHexMode()), LCD, SLOT(setHexMode()));

#ifndef NO_PLUGINS
// load plugins
  QValueList < Plugin >::Iterator mit;
  for (mit = pluginList.begin (); mit != pluginList.end (); ++mit) {
    (*mit).interface->release ();
    (*mit).library->unload ();
    delete (*mit).library;
  }
  pluginList.clear ();

  QString path = QPEApplication::qpeDir() + "/plugins/calculator";
  QDir dir (path, "lib*.so");
  QStringList list = dir.entryList ();

  QStringList::Iterator it;
  for (it = list.begin (); it != list.end (); ++it) {
    CalcInterface *iface = 0;
    OpieLibrary *lib = new OpieLibrary (path + "/" + *it);

    Plugin plugin;
    plugin.pluginWidget = 0;

    if (lib->queryInterface (IID_Calc, (QUnknownInterface **) & iface) ==
	QS_OK) {
      plugin.library = lib;
      plugin.interface = iface;
      plugin.pluginWidget = plugin.interface->getPlugin(&engine,pluginWidgetStack);
      if (plugin.pluginWidget) 
	pluginWidgetStack->addWidget (plugin.pluginWidget, pluginList.count());
      pluginList.append (plugin);
    } else {
      delete lib;
    }
  }
  setMode (1);
#else
// load simple interface
#endif
}

calc::~calc ()
{
#ifndef NO_PLUGINS
  QValueList < Plugin >::Iterator mit;
  for (mit = pluginList.begin (); mit != pluginList.end (); ++mit) {
    (*mit).interface->release ();
    (*mit).library->unload ();
    delete (*mit).library;
  }
#endif
}

