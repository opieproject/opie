#include "playerdialog.h"

#include <qgroupbox.h>

#include <qpe/qpeapplication.h>

PlayerDialog::PlayerDialog(QWidget* parent,const char* name,bool modal,WFlags f)
        :QDialog(parent,name,modal,f)
{
    auto2=false;
    auto2=true;
    setCaption("Player Settings");

    QGroupBox* player1_box=new QGroupBox("Player 1",this);
    player1_box->setGeometry(10,30,220,60);
    
    manual_button1=new QRadioButton("Human",player1_box);
    connect(manual_button1,SIGNAL(clicked()),this,SLOT(button_manual1()));
    manual_button1->setGeometry(10,20,100,20);
    auto_button1=new QRadioButton("Computer",player1_box);
    connect(auto_button1,SIGNAL(clicked()),this,SLOT(button_auto1()));
    auto_button1->setGeometry(110,20,100,20);
    button1_state(auto1);

    QGroupBox* player2_box=new QGroupBox("Player 2",this);
    player2_box->setGeometry(10,150,220,60);
    
    manual_button2=new QRadioButton("Human",player2_box);
    connect(manual_button2,SIGNAL(clicked()),this,SLOT(button_manual2()));
    manual_button2->setGeometry(10,20,100,20);
    auto_button2=new QRadioButton("Computer",player2_box);
    connect(auto_button2,SIGNAL(clicked()),this,SLOT(button_auto2()));
    auto_button2->setGeometry(110,20,100,20);
    button2_state(auto2);

    QPEApplication::showDialog( this );
}

PlayerDialog::~PlayerDialog()
{}


void PlayerDialog::button_manual1()
{
    auto1=false;
    button1_state(auto1);
}

void PlayerDialog::button_auto1()
{
    auto1=true;
    button1_state(auto1);
}

void PlayerDialog::button_manual2()
{
    auto2=false;
    button2_state(auto2);
}

void PlayerDialog::button_auto2()
{
    auto2=true;
    button2_state(auto2);
}

void PlayerDialog::button1_state(bool computer)
{
    if(computer)
    {
        manual_button1->setChecked(false);
        auto_button1->setChecked(true);
    }
    else
    {
        manual_button1->setChecked(true);
        auto_button1->setChecked(false);
    }
}

void PlayerDialog::button2_state(bool computer)
{
    if(computer)
    {
        manual_button2->setChecked(false);
        auto_button2->setChecked(true);
    }
    else
    {
        manual_button2->setChecked(true);
        auto_button2->setChecked(false);
    }
}


void PlayerDialog::setAuto1(bool newstate)
{
    auto1=newstate;
    button1_state(auto1);
}

bool PlayerDialog::getAuto1()
{
    return auto1;
}

void PlayerDialog::setAuto2(bool newstate)
{
    auto2=newstate;
    button2_state(auto2);
}

bool PlayerDialog::getAuto2()
{
    return auto2;
}
