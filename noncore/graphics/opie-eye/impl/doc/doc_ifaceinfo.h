/*
 * GPLv2
 *  zecke@handhelds.org
 */

#ifndef DOC_IFACE_INFO_H
#define DOC_IFACE_INFO_H

#include <iface/ifaceinfo.h>

class DocInterfaceInfo : public PInterfaceInfo {
public:
    DocInterfaceInfo();
    virtual ~DocInterfaceInfo();

    QString name()const;
    QWidget* configWidget(const Config&);
    void writeConfig( QWidget* wid, Config& );
};

#endif
