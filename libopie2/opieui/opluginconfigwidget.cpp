/*
 * LGPLv2 2004 zecke@handhelds.org
 */


#include "opluginconfig.h"

#include <opie/opluginloader.h>

using Opie::Core::OPluginManager;
using Opie::Core::OGenericPluginLoader;

namespace Opie {
namespace Ui   {

}
}

/**
 * C'tor compatible with all Qt Widgets in its arguments.
 * After constructing you need to  insert and call load()
 *
 * The default mode is Queued
 *
 * @param wid the Widget
 * @param name The name of the widget
 * @param fl The widget flags you may want to set
 *
 * @see QWidget::QWidget
 */
OPluginConfigWidget::OPluginConfigWidget( QWidget* wid, const char *name, WFlags fl )
        : QWidget( wid, name, fl ), m_mode( Queued )
{
}

/**
 * C'tor where you can give a Opie::Core::OPluginManager and additional optional
 * parameters.
 *
 * This call insert for the OPluginManager you pass to this method. You'll still
 * need to call load after constructing this widget.
 *
 * Ownership will not be transfered
 *
 * The default mode is Queued
 *
 * @param manager The OPluginManager you pass
 * @param wid The QWidget parent
 * @param name the Name of the widget
 * @param fl Possible Flags
 *
 */
OPluginConfigWidget::OPluginConfigWidget( Opie::Core::OPluginManager* manager,
                                          QWidget* wid, const char* name, WFlags fl )
    : QWidget( wid, name, fl ), m_mode( Queued )
{
    insert( QString::null, manager );
}


/**
 * C'tor where you can give a Opie::Core::OGenericPluginLoader and additional optional
 * parameters. Internally we use a Opie::Core::OPluginManager on top of the loader.
 *
 * This call insert for the Opie::Core::OGenericPluginLoader you pass to this method.
 * You'll still need to call load() after constructing this widget.
 *
 * It is still your OGenericPluginLoader and ownership is not transfered.
 *
 * The default mode is Queued
 *
 * @param manager The OGenericPluginLoader you pass
 * @param wid The QWidget parent
 * @param name the Name of the widget
 * @param fl Possible Flags
 *
 */
OPluginConfigWidget::OPluginConfigWidget( Opie::Core::OGenericPluginLoader* load,
                                          QWidget* wid,  const char* name, WFlags fl )
    : QWidget( wid, name, fl ), m_mode( Queued )
{
    insert( QString::null, load );
}

/**
 * d'tor. This does not save the changes if your in Queued mode
 * make sure to call save() before destructing this widget.
 */
OPluginConfigWidget::~OPluginConfigWidget() {
//
}

/**
 * Set the ChangeMode of this widget. Currently there are two possible values.
 * Immediate - all changes by the user are immediately applied and signals are emitted
 *             the signals emitted are plugin* and also changed() due calling save internally
 * Queued    - changes need to be saved manually. save() emits changed()
 *
 * @param mode the ChangeMode to use. Either use Immediate or Queued
 */
void OPluginConfigWidget::setChangeMode( enum OPluginConfigWidget::ChangeMode mode ) {
    m_mode = mode;
}

/**
 * Return the Mode
 *
 * @return Returns the change mode this widget is in.
 * @see OPluginConfigWidget::ChangeMode
 * @see OPluginConfigWidget::setChangeMode
 */
OPluginConfigWidget::ChangeMode OPluginConfigWidget::mode()const{
    return m_mode;
}

/**
 * Insert a PluginManager directly. Ownership is not transfered.
 *
 * @param name  The name represented to the user
 * @param mana  The manager which should be inserted
 */
void OPluginConfigWidget::insert( const QString& name, const Opie::Core::OPluginManager* mana) {

}

/**
 *
 * @param name The name
 *
 */
void OPluginConfigWidget::insert( const QString& name, const Opie::Core::OGenericPluginLoader* load) {

}

}
}
