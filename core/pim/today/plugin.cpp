#include "today.h"
#include <qwidget.h>
#include <qcombobox.h>
#include <oappplugin.h>

#define Q_BASEINSTANCE( i )         \
	QUnknownInterface* iface = 0;                   \
	i->queryInterface( IID_QUnknown, &iface );      \
	return iface

Q_EXPORT_INTERFACE()
{
    Today *t = new Today;
    t->setCaption( Today::tr("Today") );
    OAppPlugin *comp = new OAppPlugin(t);
    Q_BASEINSTANCE( comp );
}
