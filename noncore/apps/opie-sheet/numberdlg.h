#ifndef NUMBERDLG_H
#define NUMBERDLG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qspinbox.h>

class NumberDialog: public QDialog
{
  Q_OBJECT

  // QT objects
  QLabel *label;
  QSpinBox *edit;

  public:
    NumberDialog(QWidget *parent=0);
    ~NumberDialog();

    int exec(const QString &caption, const QString &text, int value=1, int min=1, int max=99, int step=1);
    int getValue();
};

#endif
