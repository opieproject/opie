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
class QLabel;
class QString;

#include <qdialog.h>


class SearchMethodDlg : public QDialog
{
	Q_OBJECT
	
    public:
		SearchMethodDlg(QWidget *parent, const char *name, bool modal=FALSE );

		QString itemName;

};
