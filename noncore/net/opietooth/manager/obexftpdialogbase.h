/****************************************************************************
** Form interface generated from reading ui file 'obexftpdialogbase.ui'
**
** Created: Sun Mar 19 16:46:33 2006
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef OBEXFTPDIALOGBASE_H
#define OBEXFTPDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QMultiLineEdit;
class QProgressBar;
class QPushButton;
class QTabWidget;
class QWidget;

class ObexFtpDialogBase : public QDialog
{ 
    Q_OBJECT

public:
    ObexFtpDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ObexFtpDialogBase();

    QTabWidget* obexFtpTab;
    QWidget* files;
    QListView* fileList;
    QProgressBar* fileProgress;
    QPushButton* getButton;
    QPushButton* browseOK;
    QLabel* statusBar;
    QWidget* localFs;
    QWidget* options;
    QLabel* connRetries;
    QLineEdit* nReries;
    QLabel* uuidLabel;
    QComboBox* uuidType;
    QWidget* browse;
    QMultiLineEdit* browseLog;

protected:
    QVBoxLayout* ObexFtpDialogBaseLayout;
    QVBoxLayout* filesLayout;
    QVBoxLayout* Layout9;
    QHBoxLayout* buttons;
    QHBoxLayout* browseLayout;
};

#endif // OBEXFTPDIALOGBASE_H
