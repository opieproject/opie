/****************************************************************************
** Form interface generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:32 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef __URLDIALOG_H
#define __URLDIALOG_H

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

class CURLDialog : public QDialog
{
Q_OBJECT
    QCheckBox *m_localfile, *m_globalfile, *m_clipboard;
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
 CURLDialog(const QString&, bool fs = true, QWidget* parent = 0, const char* name = 0);
    ~CURLDialog() {}
    bool clipboard() { return m_clipboard->isChecked(); }
    bool localfile() { return m_localfile->isChecked(); }
    bool globalfile() { return m_globalfile->isChecked(); }
    void clipboard(bool _b) { m_clipboard->setChecked(_b); }
    void localfile(bool _b) { m_localfile->setChecked(_b); }
    void globalfile(bool _b) { m_globalfile->setChecked(_b); }

};
#endif // CPREFS_H
