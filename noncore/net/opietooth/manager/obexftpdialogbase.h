/****************************************************************************
** Form interface generated from reading ui file 'obexftpdialogbase.ui'
**
** Created: Tue Mar 21 00:28:12 2006
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
class QListView;
class QListViewItem;
class QMultiLineEdit;
class QProgressBar;
class QPushButton;
class QSpinBox;
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
    QPushButton* browseOK;
    QPushButton* getButton;
    QPushButton* putButton;
    QLabel* statusBar;
    QWidget* localFs;
    QWidget* options;
    QLabel* uuidLabel;
    QComboBox* uuidType;
    QLabel* connRetries;
    QSpinBox* nReries;
    QWidget* browse;
    QMultiLineEdit* browseLog;

protected:
    QVBoxLayout* ObexFtpDialogBaseLayout;
    QVBoxLayout* filesLayout;
    QGridLayout* Layout13;
    QHBoxLayout* Layout11;
    QVBoxLayout* Layout5;
    QHBoxLayout* Layout3;
    QHBoxLayout* Layout4;
    QHBoxLayout* browseLayout;
};

#endif // OBEXFTPDIALOGBASE_H
