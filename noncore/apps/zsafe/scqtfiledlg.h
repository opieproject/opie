/****************************************************************************
** Form interface generated from reading ui file 'scqtfiledlg.ui'
**
** Created: Sun Jun 8 15:50:50 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SCQTFILEDLG_H
#define SCQTFILEDLG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLineEdit;
class QListView;
class QListViewItem;
class QToolButton;

class ScQtFileDlg : public QDialog
{ 
    Q_OBJECT

public:
    ScQtFileDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ScQtFileDlg();

    QComboBox* TypeComboBox;
    QToolButton* OkButton;
    QToolButton* CancelButton;
    QLineEdit* FNameLineEdit;
    QToolButton* MkDirButton;
    QComboBox* DirComboBox;
    QListView* ListView;

public slots:
    virtual void slotCancel();
    virtual void slotDirComboBoxChanged( int );
    virtual void slotDoubleClicked(QListViewItem *);
    virtual void slotFileTextChanged( const QString & );
    virtual void slotMkDir();
    virtual void slotOK();
    virtual void slotSelectionChanged(QListViewItem *);
    virtual void slotTypeComboBoxChanged( int );

protected:
    QVBoxLayout* ScQtFileDlgLayout;
    QVBoxLayout* Layout5;
    QVBoxLayout* Layout4;
    QHBoxLayout* Layout3;
    QHBoxLayout* Layout3_2;
};

#endif // SCQTFILEDLG_H
