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
/**
 * With this widget you can configure one or many PluginLoaders either
 * through an already existing Opie::Core::OPluginManager or Opie::Core::OGenericPluginLoader
 * The sorted state will be read from the Loader.
 *
 * As with Opie::Ui::OKeyConfigWidget you can have two options. To either apply your changes
 * directly through Opie::Core::OPluginManager or to use Queued where you manually need to
 * call save.
 *
 * Internally we operate on Opie::Core::OPluginManager.
 *
 * @see Opie::Ui::OKeyConfigWidget
 * @see Opie::Core::OPluginLoader
 * @see Opie::Core::OPluginManager
 *
 * @author zecke
 * @since Opie 1.1.5
 *
 */
class OPluginConfigWidget :  public QWidget {
public:
    enum ChangeMode { Queued, Immediate };
    OPluginConfigWidget( QWidget* wid = 0, const char* name = 0, WFlags fl = 0);
    OPluginConfig( Opie::Core::OPluginManager* manager, QWidget* wid = 0,
                   const char* name = 0, WFlags fl = 0);
    OPluginConfig( Opie::Core::OGenericPluginLoader* loader, QWidget* wid, const char* name,
                   WFlags fl );

    ~OPluginConfig();

    void setChangeMode( enum Mode );
    ChangeMode mode()const;

    void insert(  const QString&, const Opie::Core::OPluginManager* );
    void insert(  const QString&, const Opie::Core::OPluginLoader*  );

signals:
    /**
     * @param item The new OPluginItem
     */
    void pluginChanged ( const Opie::Core::OPluginItem& item, bool old_state);
    void pluginEnabled ( const Opie::Core::OPluginItem& item);
    void pluginDisabled( const Opie::Core::OPluginItem& item);
public:

    void load();
    void save();
private:

};
}
}

#endif
