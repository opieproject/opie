
#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H

#include <qwidget.h>


class ConfigWidget : public QWidget {
    Q_OBJECT  
public:

    ConfigWidget(  QWidget *parent,  const char *name ) : QWidget( parent,  name ) {};
    virtual ~ConfigWidget() {};

    virtual void  writeConfig() = 0;
};

#endif
