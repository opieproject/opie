
#include <qdir.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <unistd.h>
#include "cfgdlg.h"

CfgDlg::CfgDlg(QWidget* parent, CfgFile* cf, QApplication* app):QDialog(parent), cfile(cf), application(app) {
	setCaption(tr("keyz configurator"));

	QGridLayout* gl = new QGridLayout(this, 7, 5, 5, 5);

	QLabel* flabel = new QLabel("Available Keymaps", this);
	files = new QListBox(this);
	QPushButton* addButton = new QPushButton(">>", this); //add->setText("Add");
	connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
	QLabel* klabel = new QLabel("Selected Keymaps", this);
	keymaps = new QListBox(this);
	connect(keymaps, SIGNAL(highlighted(const QString&)), this, SLOT(keymapHighlighted(const QString&)));
	QPushButton* delButton = new QPushButton("<<", this); //del->setText("Delete");
	connect(delButton, SIGNAL(clicked()), this, SLOT(del()));
	QPushButton* slabelButton = new QPushButton("Set Label", this); //apply->setText("Apply");
	connect(slabelButton, SIGNAL(clicked()), this, SLOT(setLabel()));
	QLabel* l = new QLabel("Label", this);
	label = new QLineEdit(this);
	QLabel* adlabel = new QLabel("Auto Repeat Delay (ms)", this);
	QLabel* aplabel = new QLabel("Auto Repeat Period (ms)", this);
	ad = new QSpinBox(50, 5000, 5, this);
	ap = new QSpinBox(0, 5000, 5, this);
	
//	QLabel* adms = new QLabel("ms", this);
//	QLabel* apms = new QLabel("ms", this);

//	QCheckBox* enable = new QCheckBox("Enable", t);

	gl->setColStretch(0, 1);
	gl->setColStretch(1, 1);
	gl->setColStretch(3, 1);
	gl->setColStretch(4, 1);
	gl->setRowStretch(3, 1);
	gl->addMultiCellWidget(flabel, 0, 0, 0, 1);
	gl->addMultiCellWidget(klabel, 0, 0, 3, 4);
	gl->addMultiCellWidget(files, 1, 3, 0, 1);
	gl->addMultiCellWidget(keymaps, 1, 3, 3, 4);
	gl->addWidget(addButton, 1, 2);
	gl->addWidget(delButton, 2, 2);

	gl->addWidget(l, 4, 0);
	gl->addWidget(label, 4, 1);
	gl->addMultiCellWidget(slabelButton, 4, 4, 3, 4);

	gl->addMultiCellWidget(adlabel, 5, 5, 0, 2);
	gl->addMultiCellWidget(ad, 5, 5, 3, 4);
//	gl->addWidget(adms, 5, 3);

	gl->addMultiCellWidget(aplabel, 6, 6, 0, 2);
	gl->addMultiCellWidget(ap, 6, 6, 3, 4);

//	gl->addWidget(apms, 6, 3);

	QList<CfgEntry>& e = cfile->getEntries();

	for(uint i = 0; i < e.count(); i++) {
		CfgEntry* entry = e.at(i);
		keymaps->insertItem(entry->getFile());
		m.insert(entry->getFile(), entry->getLabel());
	}

	QDir dir(QPEApplication::qpeDir()+"/share/zkb", "*.xml");
	QStringList flist = dir.entryList();
	QStringList::Iterator sit;
	for(sit = flist.begin(); sit != flist.end(); ++sit) {
		if (*sit != "common.xml" && *sit != "zkb.xml" && m.find(*sit) == m.end()) {
			files->insertItem(*sit);
		}
	}

	ad->setValue(cfile->getAutorepeatDelay());
	ap->setValue(cfile->getAutorepeatPeriod());

//	gl->addMultiCellWidget(enable, 7, 7, 0, 4);
}

CfgDlg::~CfgDlg() {
}

void CfgDlg::add() {
	if (!files->currentText().isEmpty()) {
		QString t = files->currentText();
		files->removeItem(files->currentItem());
		keymaps->insertItem(t);
		cfile->replaceEntry(t, "");
		m.replace(t, "");
	}
}

void CfgDlg::del() {
	if (!keymaps->currentText().isEmpty()) {
		QString t = keymaps->currentText();
		keymaps->removeItem(keymaps->currentItem());
		cfile->deleteEntry(t);
		files->insertItem(t);
		m.remove(files->currentText());
	}
}

void CfgDlg::setLabel() {
	if (!keymaps->currentText().isEmpty()) {
		cfile->replaceEntry(keymaps->currentText(),
			label->text());
		m.replace(keymaps->currentText(), label->text());
	}
}

void CfgDlg::keymapHighlighted(const QString&text) {
	label->setText(*m.find(text));
}


void CfgDlg::accept() {
	cfile->setAutorepeatDelay(ad->value());
	cfile->setAutorepeatPeriod(ap->value());

	CfgParser p;
	p.save(QPEApplication::qpeDir()+"/share/zkb/zkb.xml", *cfile);
	QCopEnvelope("QPE/zkb", "reload()");

	QDialog::accept();
	parentWidget()->close(false);
}

