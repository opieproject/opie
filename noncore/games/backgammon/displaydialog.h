#ifndef DISPLAYDLAOG_H
#define DISPLAYDLAOG_H

#include <qcheckbox.h>
#include <qdialog.h>
#include <qradiobutton.h>

#include "definition.h"


class DisplayDialog : public QDialog
{
  Q_OBJECT
private:
  QRadioButton* small_box;
  QRadioButton* big_box;
  QCheckBox* warning_box;
public:
  DisplayDialog(QWidget* parent=0,const char* name=0,bool modal=TRUE,WFlags f=0);
  ~DisplayDialog();
private slots:
  void big_clicked();
  void small_clicked();
  void warning_clicked();
public:
  void setDisplaySettings(const Display& display);
  Display getDisplaySettings();
};

#endif //DISPLAYDLAOG_H
