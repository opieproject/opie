/****************************************************************************
**
** Created: Fri Dec 14 08:16:02 2001
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
copyright Sun 02-17-2002 22:28:23 L. J. Potter ljp@llornkcor.com
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
#include <qdict.h>

#include <qpe/filemanager.h>

#include <qvariant.h>
#include <qdialog.h>
class QLineEdit;

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QListView;
class QListViewItem;
class QPushButton;
class QComboBox;
class QWidgetStack;
class FileSelector;
class QPoint;
class MenuButton;
class QRegExp;


class fileBrowser : public QDialog
{
    Q_OBJECT

public:
    fileBrowser( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 ,const QString filter=0);
    ~fileBrowser();

    QPushButton *buttonOk, *buttonCancel, *homeButton, *docButton, *hideButton, *cdUpButton;
    QListView* ListView;

    QLabel *dirLabel;
    QString selectedFileName, filterStr;
    QDir currentDir;
    QFile file;
    QStringList fileList, dirPathStringList;
    QListViewItem * item;
    QComboBox *SelectionCombo, *dirPathCombo;
    MenuButton *typemb;
    QWidgetStack *FileStack;
    FileSelector *fileSelector;
    QString mimeType;
public slots:
        void setFileView( int );

private:
//    QDict<void> mimes;
    QRegExp tf;
    QStringList getMimeTypes();
    void fillCombo( const QString&);

private slots:
    void populateList();
    void homeButtonPushed();
    void docButtonPushed();
    void ListPressed( int, QListViewItem *, const QPoint&, int);
    void showListMenu(QListViewItem*);
    void doCd();
    void makDir();
    void localRename();
    void localDelete();
    void receive( const QCString &msg, const QByteArray &data );
    void dirPathComboActivated( const QString & );
    void upDir();
    void listClicked( QListViewItem * );
    void selectionChanged( const QString & );
    void OnOK();
    void docOpen( const DocLnk & );
    void updateMimeTypeMenu();
    void showType(const QString &);
    void dirPathEditPressed();
    
protected slots:

protected:

};


class InputDialog : public QDialog
{ 
    Q_OBJECT

public:
    InputDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~InputDialog();
    QString inputText;
    QLineEdit* LineEdit1;

};

#endif // FILEBROWSER_H
