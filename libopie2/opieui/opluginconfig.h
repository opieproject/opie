/*
 * LGPLv2 zecke@handhelds.org
 */
#ifndef ODP_OPIE_UI_OPLUGIN_CONFIG_H
#define ODP_OPIE_UI_OPLUGIN_CONFIG_H

#include <qwidget.h>

namespace Opie {
namespace Core {
    class OPluginManager;
    class OGenericPluginLoader;
}
namespace Ui {
class OPluginConfig :  public QWidget {
public:
    enum Mode { Queued, Immediate };
    OPluginConfig( QWidget* wid, const char*, WFlags fl );
    OPluginConfig( OGenericPluginManager* manager, QWidget* wid,
                   const char* name, WFlags fl );
    OPluginConfig( OPluginLoader* loader, QWidget* wid, const char* name,
                   WFlags fl );

    ~OPluginConfig();

    void setMode( enum Mode );
    Mode mode()const;

    void insert(  const OPluginManager* );
    void insert(  const OPluginLoader* , const QString& name );

    void load();
    void save();
};
}
}

#endif
