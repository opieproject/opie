/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#ifndef CALC_H
#define CALC_H

#ifdef QT_NO_COMPONENT
#define NO_PLUGINS
#endif

#include <qlayout.h>
#include <qwidgetstack.h>

#ifndef NO_PLUGINS
#include <qvaluelist.h>
#include <qpe/qlibrary.h>
#endif

#include "engine.h"
#include "plugininterface.h"

struct Plugin {
#ifndef NO_PLUGINS
  QLibrary *library;
#endif
  QWidget *pluginWidget;
  CalcInterface *interface;
    QString name;
};

class calc:public QWidget {

Q_OBJECT
public:
  calc (QWidget * p = 0, const char *n = 0);
   ~calc ();

private:
#ifndef NO_PLUGINS
  void loadPlugins ();
    QValueList < Plugin > pluginList;
#endif
  QVBoxLayout *calculatorLayout;
    QWidgetStack *pluginWidgetStack;
  QLCDNumber *LCD;
  Engine engine;

public slots:
    void setMode(int m){pluginWidgetStack->raiseWidget(m);};
};

#endif
