/****************************************************************************
**
** Created: Fri Dec 14 08:16:02 2001 fileSaver.h
**
copyright Sun 02-17-2002 22:28:48 L. J. Potter ljp@llornkcor.com

** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef FILESAVER_H
#define FILESAVER_H

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
class QListView;
class QListViewItem;
class QPushButton;
class QLineEdit;
class QCheckBox;

class fileSaver : public QDialog
{ 
    Q_OBJECT

public:
  void populateList();
    fileSaver( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 ,const QString filter=0);
    ~fileSaver();
    QLineEdit *fileEdit;

    QPushButton* buttonOk;
    QListView* ListView;
    QPushButton* buttonCancel;
    QLabel *dirLabel;
    QString selectedFileName, filterStr;
    QDir currentDir;
    QFile file;
    QStringList fileList;
    QCheckBox *filePermCheck;
 
QListViewItem * item;
public slots:

private:

private slots:
        void accept(); 
        void upDir();
        void listDoubleClicked(QListViewItem *);
        void listClicked(QListViewItem *);
        void closeEvent( QCloseEvent * );

protected slots: 

protected:

};

#endif // FILESAVER_H
