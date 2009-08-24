/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_INTERFACE_INFO_H
#define PHUNK_INTERFACE_INFO_H

#include <qstring.h>

class QWidget;
class Config;
struct PInterfaceInfo {
    virtual QString name()const = 0;
    virtual QWidget* configWidget( const Config& ) = 0;
    virtual void writeConfig(  QWidget* wid, Config& ) = 0;
};

#endif
