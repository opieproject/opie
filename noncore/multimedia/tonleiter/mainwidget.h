#ifndef TONLEITER_MAINWIDGET_H
#define TONLEITER_MAINWIDGET_H

#include <qmainwindow.h>

#include "fretboard.h"
#include "menuwidget.h"
#include "tonleiterdata.h"

class MainWidget : public QMainWindow
{
    Q_OBJECT
private:
    Data::TonleiterData* data;
    Menu::MenuWidget* menuwidget;
    Graph::FretBoard* fretboard;
public:
    MainWidget(QWidget* parent=0,const char* name="MainWidget",WFlags f=0);
    virtual ~MainWidget();
};

#endif //TONLEITER_MAINWIDGET_H
