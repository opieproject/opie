#ifndef _CONFIGDLG_H_
#define _CONFIGDLG_H_

#include "configdlg_base.h"
#include "abconfig.h"

class ConfigDlg: public ConfigDlg_Base
{
    Q_OBJECT
public:
    ConfigDlg( QWidget *parent = 0, const char *name = 0 );
    
    // Search Settings
    void setConfig( const AbConfig& cnf );
    AbConfig getConfig();

protected:
/*     virtual void itemUp(); */
/*     virtual void itemDown(); */

    QStringList contFields;
    AbConfig m_config;
};


#endif
