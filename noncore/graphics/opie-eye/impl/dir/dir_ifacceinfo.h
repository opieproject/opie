/*
 * GPLv2
 *  zecke@handhelds.org
 */

#ifndef DIR_IFACE_INFO_H
#define DIR_IFACE_INFO_H

#include <iface/ifaceinfo.h>s

class DirInterfaceInfo : public PInterfaceInfo {
public:
    DirInterfaceInfo();
    ~DirInterfaceInfo();

    QString name()const;
    QWidget* configWidget(const Config&);
    void writeConfig( QWidget* wid, Config& );
};

#endif
