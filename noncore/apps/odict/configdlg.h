/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

class QWidget;
class OTabWidget;
class QListView;
class QPushButton;

#include <qdialog.h>


class ConfigDlg : public QDialog
{
	Q_OBJECT
	
    public:
		ConfigDlg(QWidget *parent, const char *name, bool modal=FALSE );

		void writeEntries();

	private:
		OTabWidget *tab;
		QWidget *settings_tab, *search_tab;
		QListView *list;
		QPushButton *new_button, *change_button, *delete_button;

	private slots:
		void slotNewMethod();
		void slotChangeMethod();
		void slotDeleteMethod();
};
