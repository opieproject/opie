#ifndef _CONFIGDLG_H_
#define _CONFIGDLG_H_

#include <qmap.h>

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

protected slots:
    void slotItemUp();
    void slotItemDown();
    void slotItemAdd();
    void slotItemRemove();

protected:
    QStringList contFields;
    AbConfig m_config;
    QMap<QString, int> m_mapStrToID;
    QMap<int, QString> m_mapIDToStr;
};


#endif
