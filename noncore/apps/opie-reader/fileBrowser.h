/*
Derived from the file browser published by ljp@llornkcor.com but extensively
modified to work in rotated views on the Zaurus
*/

/****************************************************************************
** Form interface generated from reading ui file 'fileBrowzer.ui'
**
** Created: Fri Dec 14 08:16:02 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
blah,blah,blah
****************************************************************************/
#ifndef FILEBROWSER_H
#define FILEBROWSER_H

//#include <qvariant.h>
#include <qdialog.h>
#include <qfile.h> 
#include <qdir.h> 
#include <qstringlist.h> 
#include <qlabel.h>
#include <qstring.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QtrListView;
class QListViewItem;
class QPushButton;
class QLineEdit;

class fileBrowser : public QDialog
{ 
    Q_OBJECT

public:
  void populateList();
    fileBrowser( bool allownew, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 ,const QString filter=0, const QString iPath=0);
    ~fileBrowser();

    QPushButton* buttonOk;
    QPushButton* buttonShowHidden;
    QtrListView* ListView;
    QPushButton* buttonCancel;
    QLabel *dirLabel;
    QString selectedFileName, filterStr;
    QDir currentDir;
    QFile file;
    QString getCurrentFile();
    QLineEdit* m_filename;
    int filterspec;
//    QDir::FilterSpec filterspec;
 
//QListViewItem * item;
public slots:

private:
 QString filename; 
private slots: 
        void upDir();
        void listDoubleClicked(QListViewItem *);
        void listClicked(QListViewItem *);
	void OnRoot();
	void OnCancel();
	void setHidden(bool);
	void onReturn();

protected slots: 

protected:
        void OnOK();
 virtual void resizeEvent(QResizeEvent* e); 
};

#endif // FILEBROWSER_H
