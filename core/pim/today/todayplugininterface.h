
#ifndef TODAY_PLUGIN_INTERFACE
#define TODAY_PLUGIN_INTERFACE

#include <qpe/qcom.h>
#include "configwidget.h"

class QString;
class QWidget;

#ifndef IID_TodayPluginInterface
#define IID_TodayPluginInterface QUuid( 0x70481804, 0x2b50, 0x4fba, 0x80, 0xbb, 0x0b, 0xf8, 0xdc, 0x72, 0x04, 0x14)
#endif

/**
 *
 *
 *
 */
class TodayPluginObject  {

    Q_OBJECT

public:

    virtual ~TodayPluginObject() {};

    /**
     * The name if the plugin
     */
    virtual QString pluginName() const = 0;

    /**
     * Version numbering
     */
    virtual double versionNumber() const = 0;

    /**
     * the path and name of the identifer icon that should be shown
     * in "Resource" notation, that means only subdir/picname form $QPEDIR/pics
     */
    virtual  QString pixmapNameWidget() const = 0;

    /**    widget for the today view
     * It _needs_ a parent here.
     */
    virtual QWidget* widget( QWidget * ) = 0;

    /**
     * Pixmap used in the config widget
     */
    virtual QString pixmapNameConfig() const = 0;

    /**
     * Config plugin widget - optional
     * If the plugin has a config widget, it  _needs_  a parent here.
     */
    virtual ConfigWidget* configWidget( QWidget * ) = 0;

    /**
     * The application that should be assigned to the button (pixmap)
     */
    virtual QString appName() const = 0;

    /**
     * minimum height the plugin at least should have
     */
    virtual int minHeight()  const = 0;

    /**
     *  maximum height that should be used before starting scrolling
     */
    virtual int maxHeight() const = 0;

};


struct TodayPluginInterface : public QUnknownInterface {
    virtual TodayPluginObject *guiPart() = 0;
};

#endif
