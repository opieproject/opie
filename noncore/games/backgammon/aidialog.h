#ifndef AIDIALOG_H_
#define AIDIALOG_H_

#include "moveengine.h"

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>

class AI_Dialog : public QDialog
{
  Q_OBJECT
private:
  //GUI stuff
  QPushButton* defaultvalues;
  QSpinBox* rescue;
  QSpinBox* eliminate;
  QSpinBox* expose;
  QSpinBox* protect;
  QSpinBox* safe;
  QSpinBox* empty;
  QLabel* help;
public:
  AI_Dialog(QWidget* parent=0,const char* name=0,bool modal=TRUE,WFlags f=0);
  ~AI_Dialog();
private slots:
  void rescuehelp_pressed();
  void rescuehelp_released();
  void eliminatehelp_pressed();
  void eliminatehelp_released();
  void exposehelp_pressed();
  void exposehelp_released();
  void protecthelp_pressed();
  void protecthelp_released();
  void safehelp_pressed();
  void safehelp_released();
  void emptyhelp_pressed();
  void emptyhelp_released();
  void setDefaultValues();
public:
  void setAISettings(const AISettings& values);
  AISettings getAISettings();
private:
  void setHelpText(const QString& type="default");
};

#endif //AIDIALOG_H
