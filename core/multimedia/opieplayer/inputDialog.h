/****************************************************************************
** Form interface generated from reading ui file 'inputDialog.ui'
**
** Created: Sat Mar 2 07:54:46 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLineEdit;

class InputDialog : public QDialog
{ 
    Q_OBJECT

public:
    InputDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~InputDialog();
    QString inputText;
    QLineEdit* LineEdit1;

};

#endif // INPUTDIALOG_H
