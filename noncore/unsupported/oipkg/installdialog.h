/****************************************************************************
** Form interface generated from reading ui file 'installdialog.ui'
**
** Created: Thu May 2 22:19:01 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef INSTALLDIALOG_H
#define INSTALLDIALOG_H

#include "pksettings.h"
#include "packagelistview.h"
#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QGroupBox;
class QListView;
class QListViewItem;

class InstallDialog : public QDialog
{ 
    Q_OBJECT

public:    InstallDialog( PackageManagerSettings* s, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~InstallDialog();

    PackageListView* ListViewPackages;
    QGroupBox* GroupBoxOptions;
    QCheckBox* _force_depends;
    QCheckBox* _force_reinstall;
    QCheckBox* _force_remove;
		QCheckListItem *toRemoveItem;
		QCheckListItem *toInstallItem;

protected:
    QGridLayout* InstallDialogLayout;
    QGridLayout* GroupBoxOptionsLayout;
    bool event( QEvent* );
private:
		PackageManagerSettings* settings;
};

#endif // INSTALLDIALOG_H
