/****************************************************************************
** formatter.h
**
** Copyright: Thu Apr 11 11:01:01 2002
**      by:  L.J. Potter
**
****************************************************************************/

#ifndef FORMATTERAPP_H
#define FORMATTERAPP_H

#include <qvariant.h>
//#include <qdialog.h>
#include <qmainwindow.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QWidget;
class QStringList;

class FormatterApp : public QMainWindow
//public QDialog
{ 
    Q_OBJECT

public:
    FormatterApp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FormatterApp();

    QTabWidget *TabWidget;
    QWidget *tab, *tab_2;;
    QLabel *TextLabel1, *TextLabel2, *TextLabel3, *TextLabel4, *TextLabel5;
    QComboBox *storageComboBox, *fileSystemsCombo, *deviceComboBox;
    QPushButton *formatPushButton, *editPushButton, *fsckButton;
    QLineEdit* mountPointLineEdit;
    QStringList fileSystemTypeList, fsList, deviceList;
protected:
    QGridLayout *FormatterAppLayout, *tabLayout, *tabLayout_2;
    QString getFileSystemType(const QString &);

    void fillCombos();
    void parsetab(const QString &);
    bool doFdisk();
    int formatCheck(const QString &);
    int runCommand(const QString &);
    
protected slots:
    void cleanUp();
    void doFormat();
    void editFstab();
    bool doFsck();
    bool doFsckCheck();

    void fsComboSelected(int); 
    void storageComboSelected(int );
    void deviceComboSelected(int );
};

#endif // FORMATTERAPP_H
