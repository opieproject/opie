/*
 * examplepluginwidget.h
 *
 * email       : harlekin@handhelds.org
 *
 */

#ifndef EXAMPLE_PLUGIN_WIDGET_H
#define EXAMPLE_PLUGIN_WIDGET_H

#include <qlayout.h>

#include <opie2/oclickablelabel.h>

class ExamplePluginWidget : public QWidget {

    Q_OBJECT

public:
    ExamplePluginWidget( QWidget *parent,  const char *name );
    ~ExamplePluginWidget();
    
    void refresh();

private slots:
   void slotClicked();

private:
    Opie::Ui::OClickableLabel* m_exampleLabel;
    QHBoxLayout* m_layout;
    void readConfig();
    void getInfo();
};

#endif
