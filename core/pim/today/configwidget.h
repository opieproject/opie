
#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H


class ConfigWidget : public QWidget {


public:

    ConfigWidget( QWidget *parent,  const char *name ) : QWidget( parent,  name ) {} ;
    virtual ~ConfigWidget() {};

    virtual void  writeConfig() = 0;
};

#endif
