#include "aidialog.h"

#include <qgroupbox.h>


AI_Dialog::AI_Dialog(QWidget* parent,const char* name,bool modal,WFlags f)
  : QDialog(parent,name,modal,f)
{
    setCaption("AI Configuration");

    QGroupBox* settings=new QGroupBox("Settings",this);
    settings->setGeometry(10,5,220,170);
    
    QLabel* rescue_label=new QLabel("<b>Rescue</b>",settings);
    rescue_label->setGeometry(50,20,50,20);
    rescue=new QSpinBox(0,6,1,settings);
    rescue->setGeometry(100,20,40,20);
    QPushButton* rescuehelp=new QPushButton("?",settings);
    rescuehelp->setGeometry(140,20,20,20);
    connect(rescuehelp,SIGNAL(pressed()),this,SLOT(rescuehelp_pressed()));
    connect(rescuehelp,SIGNAL(released()),this,SLOT(rescuehelp_released()));
    
    QLabel* eliminate_label=new QLabel("<b>Eliminate</b>",settings);
    eliminate_label->setGeometry(50,40,50,20);
    eliminate=new QSpinBox(0,6,1,settings);
    eliminate->setGeometry(100,40,40,20);
    QPushButton* eliminatehelp=new QPushButton("?",settings);
    eliminatehelp->setGeometry(140,40,20,20);
    connect(eliminatehelp,SIGNAL(pressed()),this,SLOT(eliminatehelp_pressed()));
    connect(eliminatehelp,SIGNAL(released()),this,SLOT(eliminatehelp_released()));
    
    QLabel* expose_label=new QLabel("<b>Expose</b>",settings);
    expose_label->setGeometry(50,60,50,20);
    expose=new QSpinBox(0,6,1,settings);
    expose->setGeometry(100,60,40,20);
    QPushButton* exposeehelp=new QPushButton("?",settings);
    exposeehelp->setGeometry(140,60,20,20);
    connect(exposeehelp,SIGNAL(pressed()),this,SLOT(exposehelp_pressed()));
    connect(exposeehelp,SIGNAL(released()),this,SLOT(exposehelp_released()));
    
    QLabel* protect_label=new QLabel("<b>Protect</b>",settings);
    protect_label->setGeometry(50,80,50,20);
    protect=new QSpinBox(0,6,1,settings);
    protect->setGeometry(100,80,40,20);
    QPushButton* protecthelp=new QPushButton("?",settings);
    protecthelp->setGeometry(140,80,20,20);
    connect(protecthelp,SIGNAL(pressed()),this,SLOT(protecthelp_pressed()));
    connect(protecthelp,SIGNAL(released()),this,SLOT(protecthelp_released()));
    
    QLabel* safe_label=new QLabel("<b>Safe</b>",settings);
    safe_label->setGeometry(50,100,50,20);
    safe=new QSpinBox(0,6,1,settings);
    safe->setGeometry(100,100,40,20);
    QPushButton* safeehelp=new QPushButton("?",settings);
    safeehelp->setGeometry(140,100,20,20);
    connect(safeehelp,SIGNAL(pressed()),this,SLOT(safehelp_pressed()));
    connect(safeehelp,SIGNAL(released()),this,SLOT(safehelp_released()));
    
    QLabel* empty_label=new QLabel("<b>Empty</b>",settings);
    empty_label->setGeometry(50,120,50,20);
    empty=new QSpinBox(0,6,1,settings);
    empty->setGeometry(100,120,40,20);
    QPushButton* emptyhelp=new QPushButton("?",settings);
    emptyhelp->setGeometry(140,120,20,20);
    connect(emptyhelp,SIGNAL(pressed()),this,SLOT(emptyhelp_pressed()));
    connect(emptyhelp,SIGNAL(released()),this,SLOT(emptyhelp_released()));
    
    defaultvalues=new QPushButton("Default Values",settings);
    defaultvalues->setGeometry(60,140,90,20);
    connect(defaultvalues,SIGNAL(clicked()),this,SLOT(setDefaultValues()));

    QGroupBox* helpbox=new QGroupBox("Help",this);
    helpbox->setGeometry(10,185,220,90);

    help=new QLabel(helpbox);
    help->setGeometry(10,15,200,65);
    
    setHelpText();
    showMaximized();
}


AI_Dialog::~AI_Dialog()
{
}

void AI_Dialog::rescuehelp_pressed()
{
  setHelpText("rescue");
}

void AI_Dialog::rescuehelp_released()
{
  setHelpText();
}

void AI_Dialog::eliminatehelp_pressed()
{
  setHelpText("eliminate");
}

void AI_Dialog::eliminatehelp_released()
{
  setHelpText();
}

void AI_Dialog::exposehelp_pressed()
{
  setHelpText("expose");
}

void AI_Dialog::exposehelp_released()
{
  setHelpText();
}

void AI_Dialog::protecthelp_pressed()
{
  setHelpText("protect");
}

void AI_Dialog::protecthelp_released()
{
  setHelpText();
}

void AI_Dialog::safehelp_pressed()
{
  setHelpText("safe");
}

void AI_Dialog::safehelp_released()
{
  setHelpText();
}

void AI_Dialog::emptyhelp_pressed()
{
  setHelpText("empty");
}

void AI_Dialog::emptyhelp_released()
{
  setHelpText();
}

  

void AI_Dialog::setDefaultValues()
{
    rescue->setValue(6);
    eliminate->setValue(4);
    expose->setValue(1);
    protect->setValue(5);
    safe->setValue(3);
    empty->setValue(2);
}

void AI_Dialog::setAISettings(const AISettings& values)
{
    rescue->setValue(values.rescue);
    eliminate->setValue(values.eliminate);
    expose->setValue(values.expose);
    protect->setValue(values.protect);
    safe->setValue(values.safe);
    empty->setValue(values.empty);
}

AISettings AI_Dialog::getAISettings()
{
    AISettings ai;
    ai.rescue=rescue->value();
    ai.eliminate=eliminate->value();
    ai.expose=expose->value();
    ai.protect=protect->value();
    ai.safe= safe->value();
    ai.empty=empty->value();    
    return ai;
}

void AI_Dialog::setHelpText(const QString& type)
{
  if(type=="rescue")
  {
    help->setText("Bring the pieces out of the endzone");  
  }
  else if(type=="eliminate")
  {
    help->setText("Eliminate an opponents piece");
  }
  else if(type=="expose")
  {
    help->setText("Expose you own pieces.\nAfter such a move only one piece will remain in the slot");  
  }
  else if(type=="protect")
  {
    help->setText("Protect a single piece by\nputting another one in this slot");
  }
  else if(type=="safe")
  {
    help->setText("Move piece to a slot already\noccupied by the player");  
  }
  else if(type=="empty")
  {
    help->setText("Move piece to an empty slot");
  }
  else
  {
    help->setText("Press and hold the ? buttton\nnext to a field for help");
  }
}
