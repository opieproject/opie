
#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H


/**
 * A base class for all Today Config Widgets.
 * This will let a Today plugin to add the possibility of configuration.
 * Plugins need to inherit from this class and need to implement
 * the pure virtual method to control configuration.
 * The Plugin should read its configuration during creation of the Widget
 *
 *
 * @author Maximilian Reiﬂ
 * @short base class of all today config widgets
 */
class TodayConfigWidget : public QWidget {


public:

    /**
     * This will construct the widget. The widget gets deleted once the parent
     * gets deleted as in any Qt application
     *
     * @param parent The parent of the widget
     * @param name The name of the object
     */
    TodayConfigWidget( QWidget *parent,  const char *name ) : QWidget( parent,  name ) {} ;
    virtual ~TodayConfigWidget() {};

    /**
     * Plugins need to reimplement this in the config widget
     * Used when the config dialog is closed to write config stuff
     */
    virtual void  writeConfig() = 0;
};

#endif
