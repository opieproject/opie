#include "displaydialog.h"

#include <qgroupbox.h>
#include <qlabel.h>

DisplayDialog::DisplayDialog(QWidget* parent,const char* name,bool modal,WFlags f)
  : QDialog(parent,name,modal,f)
{
    setCaption("Display Configuration");
    QLabel* header=new QLabel("<b>Change the display settings</b>",this);
    header->setGeometry(10,10,200,20);

    //
    QGroupBox* settings_frame=new QGroupBox("Settings",this);
    settings_frame->setGeometry(10,10,220,120);
    
    small_box=new QRadioButton("Big display, e.g. for QPE Style",settings_frame);
    small_box->setGeometry(10,20,200,20);
    connect(small_box,SIGNAL(clicked()),this,SLOT(small_clicked()));
   
    big_box=new QRadioButton("Small display, e.g. for Windows Style",settings_frame);
    big_box->setGeometry(10,50,200,20);
    connect(big_box,SIGNAL(clicked()),this,SLOT(big_clicked()));

    //
    QGroupBox* warning_frame=new QGroupBox("Warning",this);
    warning_frame->setGeometry(10,140,220,120);

    warning_box=new QCheckBox("show style warning",warning_frame);
    warning_box->setGeometry(10,20,200,20);
    connect(warning_box,SIGNAL(clicked()),this,SLOT(warning_clicked()));
    
    QLabel* warning_help=new QLabel("show style warning at statup if the system style is not QPE\nif not set to small you may have\nscrollbars on the palying area",warning_frame);
    warning_help->setGeometry(10,50,200,60);
    
    showMaximized();
}


DisplayDialog::~DisplayDialog()
{
}

void DisplayDialog::big_clicked()
{
  big_box->setChecked(true);
  small_box->setChecked(false);
}

void DisplayDialog::small_clicked()
{
  big_box->setChecked(false);
  small_box->setChecked(true);
}

void DisplayDialog::warning_clicked()
{
}

void DisplayDialog::setDisplaySettings(const Display& display)
{
  if(!display.small)
  {
    big_box->setChecked(false);
    small_box->setChecked(true);
  }
  else
  {
    big_box->setChecked(true);
    small_box->setChecked(false);
  }
  if(display.warning)
    warning_box->setChecked(true);
  else
    warning_box->setChecked(false);

}

Display DisplayDialog::getDisplaySettings()
{
  Display display;
  display.small=!small_box->isChecked();
  display.warning=warning_box->isChecked();
  return display;
}


  

