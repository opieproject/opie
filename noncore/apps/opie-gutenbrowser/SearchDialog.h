/****************************************************************************
** Form interface generated from reading ui file 'SearchDialog.ui'
**
** Created: Tue Aug 29 11:12:09 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
****************************************************************************/
#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <qdialog.h>
#include "SearchResults.h"
#include <qpe/config.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;

class SearchDialog : public QDialog
{ 
    Q_OBJECT

public:
    SearchDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SearchDialog();

    QLabel* TextLabel1;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLineEdit* SearchLineEdit;
    QString searchString;
    QString label1Str;
    QCheckBox *caseSensitiveCheckBox;
Config *cfg;
protected:
    QHBoxLayout* hbox;

public slots: // Public slots
  void setLabel( QString labelText);
 void byeBye();
 bool get_direction();
 bool case_sensitive();
 bool forward_search();
 //void setLabel();
    void closed();
 QString get_text();
signals:

 void search_signal();
 void search_done_signal();
};

#endif // SEARCHDIALOG_H
