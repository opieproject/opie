#include "cfgfile.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qmessagebox.h>

// CfgEntry implementation
CfgEntry::CfgEntry() {
}

CfgEntry::CfgEntry(const QString& f, const QString& l):
	file(f), label(l) {
}

const QString& CfgEntry::getFile() const {
	return file;
}

void CfgEntry::setFile(const QString& f) {
	file = f;
}

const QString& CfgEntry::getLabel() const {
	return label;
}

void CfgEntry::setLabel(const QString& f) {
	label = f;
}

// CfgFile implementation
CfgFile::CfgFile():ardelay(400), arperiod(80) {
}

CfgFile::~CfgFile() {
}

QList<CfgEntry>& CfgFile::getEntries() {
	return entries;
}

bool CfgFile::replaceEntry(const QString& file, const QString& label, int index) {
	deleteEntry(file);

	CfgEntry* entry = new CfgEntry(file, label);
	if (index >= 0) {
		entries.insert(index, entry);
	} else {
		entries.append(entry);
	}

	return true;
}

bool CfgFile::deleteEntry(const QString& file) {
	for(int i = 0; i < (int) entries.count(); i++) {
		CfgEntry* entry = entries.at(i);
		if (entry->getFile() == file) {
			entries.remove(i);
			return true;
		}
	}
	return false;
}

int CfgFile::getAutorepeatDelay() const {
	return ardelay;
}

void CfgFile::setAutorepeatDelay(int n) {
	ardelay = n;
}

int CfgFile::getAutorepeatPeriod() const {
	return arperiod;
}

void CfgFile::setAutorepeatPeriod(int n) {
	arperiod = n;
}

// CfgParser implementation
CfgParser::CfgParser() {
}

bool CfgParser::load(QString file, CfgFile& cfg) {
	QFile f(file);
	QXmlInputSource is(f);
	QXmlSimpleReader reader;
	CfgHandler p(*this);
	
	reader.setErrorHandler(this);
	reader.setContentHandler(&p);

	err = "";
	ardelay = -1;
	arperiod = -1;
	reader.parse(is);

	if (!err.isEmpty()) {
		odebug << err << oendl; 
		return false;
	}

	QMap<QString, QString>::Iterator fit, lit;
	for(uint i = 0; i < includeList.count(); i++) {
		QString file = *includeList.at(i);
		fit = includes.find(file);
		QString prefix = fit.data();
		QString label = "";

		odebug << "include: file=" + fit.key() + ", prefix=" + fit.data() << oendl; 
		lit = labels.find(prefix+":*");
		if (lit != labels.end()) {
			label = lit.data();
		}

		cfg.replaceEntry(file, label);
	}

	if (ardelay != -1) {
		cfg.setAutorepeatDelay(ardelay);
	}

	if (arperiod != -1) {
		cfg.setAutorepeatPeriod(arperiod);
	}

	return true;
}

bool CfgParser::save(QString file, CfgFile& cfg) {
	FILE* f = fopen((const char*) file.local8Bit(), "w");

	fprintf(f, "<keymap autorepeat-delay=\"%d\" autorepeat-period=\"%d\" "
		"author=\"keyzcfg\">\n", cfg.getAutorepeatDelay(), 
		cfg.getAutorepeatPeriod());

	QList<CfgEntry>& entries = cfg.getEntries();
	int n;

	for(n=0; n < (int) entries.count(); n++) {
		CfgEntry* entry = entries.at(n);
		QString l = entry->getLabel();
		if (!l.isEmpty()) {
			fprintf(f, "\t<label name=\"%s\" state=\"km%d:*\"/>\n", 
				(const char*) l.utf8(), n);
		}
	}

	for(n=0; n < (int) entries.count(); n++) {
		CfgEntry* entry = entries.at(n);
		fprintf(f, "\t<include file=\"%s\" prefix=\"km%d\"/>\n",
			(const char*) entry->getFile().utf8(), n);
	}

	int k = n-1;
	char* states[] = { "LShift", "LShift-Caps", "LShift-Num",
		"LShift-Num-Caps", 0};

	for(n=0; n < (int) entries.count(); n++) {
		QString nstate = "km" + QString::number(n+1);
		if (n == k) {
			nstate = "km" + QString::number(0);
		}

		for(int i = 0; states[i] != 0; i++) {
			fprintf(f, "\t<state name=\"km%d:%s\">\n",
				n, states[i]);
			fprintf(f, "\t\t<map keycode=\"Middle\" pressed=\"true\">\n");
			fprintf(f, "\t\t\t<next-state name=\"%s:%s\"/>\n", 
				(const char*) nstate.utf8(), states[i]);
			fprintf(f, "\t\t</map>\n\t</state>\n\n");
		}
	}

	fprintf(f, "\t<state name=\"km0:Normal\" default=\"true\"/>\n");

	fprintf(f, "</keymap>");
	fclose(f);
	return true;
}

CfgParser::~CfgParser() {
}

int CfgParser::getAutorepeatDelay() const {
	return ardelay;
}

void CfgParser::setAutorepeatDelay(int n) {
	ardelay = n;
}

int CfgParser::getAutorepeatPeriod() const {
	return arperiod;
}

void CfgParser::setAutorepeatPeriod(int n) {
	arperiod = n;
}

void CfgParser::addLabel(const QString& name, const QString& state) {
	labels.insert(state, name);
	labelList.append(&labels.find(state).data());
}

void CfgParser::addFile(const QString& file, const QString& prefix) {
	includes.insert(file, prefix);
	includeList.append(&includes.find(file).key());
}

QString CfgParser::errorString() {
	return err;
}

bool CfgParser::warning(const QXmlParseException& e) {
	QString tmp;

	tmp.sprintf("%d: warning: %s\n", e.lineNumber(), 
		(const char*) e.message().utf8());

	err += tmp;

	return true;
}

bool CfgParser::error(const QXmlParseException& e) {
	QString tmp;

	tmp.sprintf("%d: error: %s\n", e.lineNumber(), 
		(const char*) e.message().utf8());

	err += tmp;

	return true;
}

bool CfgParser::fatalError(const QXmlParseException& e) {
	QString tmp;

	tmp.sprintf("%d: fatal error: %s\n", e.lineNumber(), 
		(const char*) e.message().utf8());

	err += tmp;

	return false;
}

// CfgHandler implementation
CfgHandler::CfgHandler(CfgParser& c):cfg(c) {
}

CfgHandler::~CfgHandler() {
}

bool CfgHandler::startKeymapElement(int ard, int arp, const QString& author) {
	if (author != "keyzcfg") {
		bool ret;
		ret = QMessageBox::warning(0, "keyz configurator", 
			"Your zkb.xml doesn't seem created by keyz configurator.\n"
			"By using keyz configurator you may loose your current "
			"configuration\n Do you want to continue\n\n",
			"Yes", "No", 0, 0, 1);

		if (ret != 0) {
			err = "cancelled by user";
			return false;
		}
	}

	if (ard != -1) {
		cfg.setAutorepeatDelay(ard);
	}

	if (arp != -1) {
		cfg.setAutorepeatPeriod(arp);
	}

	return true;
}

bool CfgHandler::startIncludeElement(const QString& file, 
	const QString& pref) {

	cfg.addFile(file, pref);
	return true;
}

bool CfgHandler::startLabelElement(const QString& label, 
	const QString& state) {

	cfg.addLabel(label, state);
	return true;
}

bool CfgHandler::startStateElement(const QString&, const QString&, bool) {

	return true;
}

bool CfgHandler::startMapElement(int, bool) {
	return true;
}

bool CfgHandler::startEventElement(int, int, int, bool, bool) {
	return true;
}

bool CfgHandler::startNextStateElement(const QString&) {
	return true;
}


bool CfgHandler::endKeymapElement() {
	return true;
}

bool CfgHandler::endIncludeElement() {
	return true;
}

bool CfgHandler::endLabelElement() {
	return true;
}

bool CfgHandler::endStateElement() {
	return true;
}

bool CfgHandler::endMapElement() {
	return true;
}

bool CfgHandler::endEventElement() {
	return true;
}

bool CfgHandler::endNextStateElement() {
	return true;
}
