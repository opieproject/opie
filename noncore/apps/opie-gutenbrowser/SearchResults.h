
/****************************************************************************
** Created: Sat Feb 24 23:32:10 2001
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com

****************************************************************************/
#ifndef SEARCHRESULTSDLG_H
#define SEARCHRESULTSDLG_H

//#include "gutenbrowser.h"
#include <qdialog.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qlistbox.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
//class QListBox;
class QListBoxItem;
class QPushButton;

class SearchResultsDlg : public QDialog
{ 
    Q_OBJECT

public:
    SearchResultsDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, QStringList stringList=0);
    ~SearchResultsDlg();

    QLabel* statusLabel, *outPutLabel;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QListBox* ListBox1;
    QString selText;
    QStringList resultsList;
    int numListItems;

protected:
    QHBoxLayout* hbox;
public slots:
    void slotListClicked(QListBoxItem *);

protected slots:
    void dwnLd();
    void downloadButtonSlot();

};

#endif // SEARCHRESULTSDLG_H
