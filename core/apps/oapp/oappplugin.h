#ifndef __OAPPPLUGIN_H
#define __OAPPPLUGIN_H

#include "oappinterface.h"
#include <qlist.h>
#include <qpe/quuid.h>

class QWidget;

class OAppPlugin : public OAppInterface
{
public:
    OAppPlugin(OAppPos pos = midPos);
    OAppPlugin(QWidget *widget, OAppPos pos = midPos);
    virtual ~OAppPlugin();

    QList<QWidget> widgets();
    OAppPos position() const;

#ifndef QT_NO_COMPONENT
    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface );
    Q_REFCOUNT
#endif

private:
    QList<QWidget> m_widgets;
    OAppPos m_position;
};

#endif // __OAPPPLUGIN_H
