
#ifndef TODAY_PLUGIN_INTERFACE
#define TODAY_PLUGIN_INTERFACE

#include <qpe/qcom.h>
#include "todayconfigwidget.h"

class QString;
class QWidget;

#ifndef IID_TodayPluginInterface
#define IID_TodayPluginInterface QUuid( 0x70481804, 0x2b50, 0x4fba, 0x80, 0xbb, 0x0b, 0xf8, 0xdc, 0x72, 0x04, 0x14)
#endif

/**
 *
 * A TodayPluginObject is the base for all Today Plugins.
 * A plugin author needs to inherit this class and implement
 * the pure virtual methods
 *
 * @short base class for today plugins
 * @author Maximilian Reiss
 *
 */
class TodayPluginObject {

public:

    virtual ~TodayPluginObject() {};

    /**
     * The name if the plugin
     * @return The plugin should return its name here
     */
    virtual QString pluginName() const = 0;

    /**
     * Version numbering
     * @return The plugin should return the version number
     */
    virtual double versionNumber() const = 0;


    /**
     * @return the pixmap name widget?! -- FIXME
     */
    virtual  QString pixmapNameWidget() const = 0;

    /**
     * widget for the today view
     * It _needs_ a parent here.
     * Plugin authors need to take parent as parent!
     */
    virtual QWidget* widget( QWidget *parent ) = 0;

    /**
     * Pixmap used in the config widget
     */
    virtual QString pixmapNameConfig() const = 0;

    /**
     * Config plugin widget - optional
     * If the plugin has a config widget, it  _needs_  a parent here.
     * may return 0 if no config widget is needed
     */
    virtual TodayConfigWidget* configWidget( QWidget * ) = 0;

    /**
     * The application that should be assigned to the button (pixmap)
     * Today will show the plugin icon. On click it tries to execute the
     * plugin related application.
     */
    virtual QString appName() const = 0;


    /**
     * If the plugin should take part in the periodic refresh
     */
    virtual bool excludeFromRefresh() const = 0;

    /**
     * Refresh that plugins view. For updating the plugins
     *
     */
    virtual void refresh() {};

    /**
     * minimum height the plugin at least should have
     */
    // virtual int minHeight()  const = 0;

    /**
     *  maximum height that should be used before starting scrolling
     */
    // virtual int maxHeight() const = 0;

};

/**
 * This is part of the QCOM works. See example plugins how to do it right
 */
struct TodayPluginInterface : public QUnknownInterface {
    /**
     * return the TodayPluginObject implementation
     */
    virtual TodayPluginObject *guiPart() = 0;
};

#endif
