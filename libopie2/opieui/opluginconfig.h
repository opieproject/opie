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
 * Internally we operate on Opie::Core::OPluginManager. You can insert any numbers of loaders
 * or pluginmanagers.  But you need to call load() after you've inserted new items.
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

    virtual ~OPluginConfig();

    void setChangeMode( enum ChangeMode );
    ChangeMode mode()const;

    void insert(  const QString&, const Opie::Core::OPluginManager* );
    void insert(  const QString&, const Opie::Core::OGenericPluginLoader*  );

signals:
    /**
     * A PluginItem was changed. This signal is only emitted if you're
     * in the immediate ChangeMode.
     * This is emitted on any change ( disable,enable,pos)
     *
     * @param item The OPluginItem that was changed. This is how it looks now
     * @param old_state If  it was enabled before
     */
    void pluginChanged ( const Opie::Core::OPluginItem& item, bool old_state);

    /**
     * emitted only if you're in Immediate when an Item was enabled
     *
     * @param item The Item that was enabled
     */
    void pluginEnabled ( const Opie::Core::OPluginItem& item);

    /**
     * emitted only if you're in Immediate when an Item was enabled
     *
     * @param item The Item was disabled
     */
    void pluginDisabled( const Opie::Core::OPluginItem& item);

    /**
     * Data was saved(). Normally this is emitted when save() is called
     * @see changed
     */
    void changed();
public:

    void load();
    void save();
private:
    ChangeMode m_mode;
    QMap<Opie::Core::OPluginManager*, bool> m_items;

};
}
}

#endif
