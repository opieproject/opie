/*
 * GPLv2
 *  zecke@handhelds.org
 */

#ifndef DCIM_IFACE_INFO_H
#define DCIM_IFACE_INFO_H

#include <iface/ifaceinfo.h>

class DCIM_InterfaceInfo : public PInterfaceInfo {
public:
    DCIM_InterfaceInfo();
    virtual ~DCIM_InterfaceInfo();

    QString name()const;
    QWidget* configWidget( const Config& );
    void writeConfig( QWidget* wid, Config& );
};


#endif
