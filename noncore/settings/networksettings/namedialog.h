/****************************************************************************
** Form interface generated from reading ui file 'namedialog.ui'
**
** Created: Sat Dec 15 04:36:14 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLineEdit;
class QPushButton;

class NameDialog : public QDialog
{ 
    Q_OBJECT

public:
    NameDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );

  QLineEdit* Name;
  QPushButton* cancel;
  QPushButton* ok;
  QString go();

protected:
    QGridLayout* NameDialogLayout;
};

#endif // NAMEDIALOG_H
