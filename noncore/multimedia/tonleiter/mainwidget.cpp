#include "mainwidget.h"

#include <qwidget.h>
#include <qlayout.h>

MainWidget::MainWidget(QWidget* parent,const char* name,WFlags f)
:QMainWindow(parent,name,f)
{
    setCaption("Tonleiter");
    data=new Data::TonleiterData(this);

    QWidget* mainwidget=new QWidget(this,"mainwidget");
    setCentralWidget(mainwidget);
    QBoxLayout* mainlayout=new QBoxLayout(mainwidget,QBoxLayout::TopToBottom);

    menuwidget=new Menu::MenuWidget(data,mainwidget);
    mainlayout->addWidget(menuwidget);

    fretboard=new Graph::FretBoard(data,mainwidget);
    mainlayout->addWidget(fretboard);

}
//****************************************************************************
MainWidget::~MainWidget()
{
}
//****************************************************************************
//****************************************************************************
