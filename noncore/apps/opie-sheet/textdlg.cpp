#include "textdlg.h"

TextDialog::TextDialog(QWidget *parent=0)
           :QDialog(parent, 0, TRUE)
{
  edit=new QLineEdit(this);
  edit->setGeometry(90, 10, 100, 25);

  label=new QLabel(this);
  label->setGeometry(10, 10, 70, 25);
  label->setBuddy(edit);

  resize(200, 45);
}

TextDialog::~TextDialog()
{
}

int TextDialog::exec(const QString &caption, const QString &text, const QString &value="")
{
  setCaption(caption);
  label->setText(text);
  edit->setText(value);

  return QDialog::exec();
}

QString TextDialog::getValue()
{
  return edit->text();
}
