/****************************************************************************
** Form interface generated from reading ui file 'app.ui'
**
** Created: Fri May 3 14:38:09 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef APPEDIT_H
#define APPEDIT_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;

class AppEdit : public QDialog
{
    Q_OBJECT

public:
    AppEdit( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AppEdit();

    QLabel* TextLabel3;
    QLabel* TextLabel4;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QLineEdit* iconLineEdit;
    QLineEdit* nameLineEdit;
    QLineEdit* commentLineEdit;
    QLineEdit* execLineEdit;

protected:
    QGridLayout* AppEditLayout;
};

#endif // APPEDIT_H
