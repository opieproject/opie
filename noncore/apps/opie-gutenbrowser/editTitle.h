/****************************************************************************
** Created: Thu Jan 24 08:47:13 2002
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
****************************************************************************/
#ifndef EDIT_TITLE_H
#define EDIT_TITLE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLineEdit;
class QPushButton;

class Edit_Title : public QDialog
{ 
    Q_OBJECT

public:
    Edit_Title( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~Edit_Title();

    QLineEdit* LineEdit1;
    QPushButton* PushButton_Cancel;
    QPushButton* PushButton_Apply;
    QString newTitle;
public slots:
    void onCancel();
    void onApply(); 
};

#endif // EDIT_TITLE_H
