#ifndef RULESDIALOG_H_
#define RULESDIALOG_H_

#include <qcheckbox.h>
#include <qdialog.h>
#include "definition.h"


class RulesDialog : public QDialog
{
  Q_OBJECT
private:
  QCheckBox* pieces_out;
  QCheckBox* nice_dice;
  Rules rules;
public:
  RulesDialog(QWidget* parent=0,const char* name=0,bool modal=TRUE,WFlags f=0);
  ~RulesDialog();
private slots:
  void pieces_out_clicked();
  void nice_dice_clicked();
public:
  void setRules(const Rules& new_rules);
  Rules getRules();
};

#endif //RULESDIALOG_H
