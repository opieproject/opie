
#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H


class TodayConfigWidget : public QWidget {


public:

    TodayConfigWidget( QWidget *parent,  const char *name ) : QWidget( parent,  name ) {} ;
    virtual ~TodayConfigWidget() {};

    /**
     * Plugins need to reimplement this in the config widget
     * Used when the config dialog is closed to write config stuff
     */
    virtual void  writeConfig() = 0;
};

#endif
