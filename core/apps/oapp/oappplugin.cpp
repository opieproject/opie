#include "oappplugin.h"
#include <qwidget.h>

OAppPlugin::OAppPlugin(OAppPos pos)
{
    m_position = pos;
};

OAppPlugin::OAppPlugin(QWidget *widget, OAppPos pos)
{
    m_widgets.append( widget );
    m_position = pos;
};

OAppPlugin::~OAppPlugin()
{
};

QList<QWidget> OAppPlugin::widgets()
{
    return m_widgets;
};

OAppPos OAppPlugin::position() const
{
    return m_position;
}

QRESULT OAppPlugin::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
        *iface = this;
    else if ( uuid == IID_OAppInterface )
        *iface = this;

    if ( *iface )
        (*iface)->addRef();
    return QS_OK;
}
