#include "rulesdialog.h"

#include <qgroupbox.h>
#include <qlabel.h>

RulesDialog::RulesDialog(QWidget* parent,const char* name,bool modal,WFlags f)
  : QDialog(parent,name,modal,f)
{
    setCaption("Rules Configuration");
    QLabel* header=new QLabel("<b>Change the game rules here</b>",this);
    header->setGeometry(10,10,200,20);

    //
    QGroupBox* pieces_out_box=new QGroupBox("Movement",this);
    pieces_out_box->setGeometry(10,10,220,120);
    
    pieces_out=new QCheckBox("Don't care about others",pieces_out_box);
    pieces_out->setGeometry(10,20,200,20);
    connect(pieces_out,SIGNAL(clicked()),this,SLOT(pieces_out_clicked()));
   
    QLabel* pieces_out_help=new QLabel("allow movement of the pieses\neven if there are pieces knocked\nout by the opponent",pieces_out_box);
    pieces_out_help->setGeometry(10,40,200,60);

    //
    QGroupBox* nice_dice_box=new QGroupBox("Dice",this);
    nice_dice_box->setGeometry(10,140,220,120);

    nice_dice=new QCheckBox("Big dice for small numbers",nice_dice_box);
    nice_dice->setGeometry(10,20,200,20);
    connect(nice_dice,SIGNAL(clicked()),this,SLOT(nice_dice_clicked()));

    QLabel* nice_dice_help=new QLabel("allow to rescue pieces with dice\nvalues graeter than the distance\nto the players endzone.",nice_dice_box);
    nice_dice_help->setGeometry(10,40,200,60);
    
    showMaximized();
}


RulesDialog::~RulesDialog()
{
}

void RulesDialog::pieces_out_clicked()
{
  if(pieces_out->isChecked())
    rules.move_with_pieces_out=true;
  else
    rules.move_with_pieces_out=false;
}

void RulesDialog::nice_dice_clicked()
{
  if(nice_dice->isChecked())
    rules.generous_dice=true;
  else
    rules.generous_dice=false;
}

void RulesDialog::setRules(const Rules& new_rules)
{
  rules=new_rules;
  if(rules.move_with_pieces_out)
    pieces_out->setChecked(true);
  else
    pieces_out->setChecked(false);

  if(rules.generous_dice)
    nice_dice->setChecked(true);
  else
    nice_dice->setChecked(false);    
}

Rules RulesDialog::getRules()
{
  return rules;
}

