#ifndef TEXTDLG_H
#define TEXTDLG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>

class TextDialog: public QDialog
{
  Q_OBJECT

  // QT objects
  QLabel *label;
  QLineEdit *edit;

  public:
    TextDialog(QWidget *parent=0);
    ~TextDialog();

    int exec(const QString &caption, const QString &text, const QString &value="");
    QString getValue();
};

#endif
