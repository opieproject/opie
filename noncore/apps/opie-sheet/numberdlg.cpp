#include "numberdlg.h"

NumberDialog::NumberDialog(QWidget *parent=0)
             :QDialog(parent, 0, TRUE)
{
  edit=new QSpinBox(this);
  edit->setGeometry(120, 10, 70, 25);

  label=new QLabel(this);
  label->setGeometry(10, 10, 100, 25);
  label->setBuddy(edit);

  resize(200, 45);
}

NumberDialog::~NumberDialog()
{
}

int NumberDialog::exec(const QString &caption, const QString &text, int value=1, int min=1, int max=99, int step=1)
{
  setCaption(caption);
  label->setText(text);
  edit->setValue(value);
  edit->setMinValue(min);
  edit->setMaxValue(max);
  edit->setLineStep(step);

  return QDialog::exec();
}

int NumberDialog::getValue()
{
  return edit->value();
}
