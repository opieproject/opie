/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>


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
