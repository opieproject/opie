#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qdialog.h>
#include <qapplication.h>

#include "cfgfile.h"

class CfgDlg : public QDialog {
Q_OBJECT

public:
	CfgDlg(QWidget* parent, CfgFile* cfile, QApplication *);
	virtual ~CfgDlg();

protected:
	CfgFile* cfile;
	QApplication* application;

	QMap<QString, QString> m;

	QListBox* files;
	QListBox* keymaps;
	QLineEdit* label;
	QSpinBox* ap;
	QSpinBox* ad;

	virtual void accept();

protected slots:
	void add();
	void del();
	void setLabel();
	void keymapHighlighted(const QString&);
};
