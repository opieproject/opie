/****************************************************************************
** Form interface generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:32 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef __CLOSEDIALOG_H
#define __CLOSEDIALOG_H

#include "useqpe.h"
#include <qvariant.h>
#include <qwidget.h>
#include <qtabdialog.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
//#include <qpe/menubutton.h>
#include <qvbuttongroup.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
//class QCheckBox;
class QLabel;
//class QSpinBox;

class CCloseDialog : public QDialog
{
Q_OBJECT
    QCheckBox *file, *marks, *config;
#ifndef USEQPE
    void keyPressEvent(QKeyEvent* e)
	{
	    switch (e->key())
	    {
		case Key_Escape:
		    e->accept();
		    reject();
		    break;
		case Key_Space:
		case Key_Return:
		    e->accept();
		    accept();
		    break;
		default:
		    QWidget::keyPressEvent(e);
	    }
	}
#endif
 public:
 CCloseDialog(const QString&, bool fs = true, QWidget* parent = 0, const char* name = 0);
    ~CCloseDialog() {}
    bool delFile() { return file->isChecked(); }
    bool delMarks() { return marks->isChecked(); }
    bool delConfig() { return config->isChecked(); }

};
#endif // CPREFS_H
