#include "zkbcfg.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qfileinfo.h>

// Implementation of XkbConfig class
ZkbConfig::ZkbConfig(const QString& dir):path(dir) {
}

ZkbConfig::~ZkbConfig() {
}

bool ZkbConfig::load(const QString& file, Keymap& keymap, const QString& prefix) {
	bool ret;
	QFile f(path+"/"+file);
	QFileInfo fi(f);

	odebug << "start loading file=" << file.utf8() << "\n" << oendl; 
	if (includedFiles.find(fi.absFilePath()) != includedFiles.end()) {
		return false;
	}

	includedFiles.insert(fi.absFilePath(), 1);
	QXmlInputSource is(f);
	QXmlSimpleReader reader;
	ZkbHandler h(*this, keymap, prefix);

	reader.setContentHandler(&h);
	reader.setErrorHandler(this);
	ret = reader.parse(is);
	includedFiles.remove(fi.absFilePath());

	odebug << "end loading file=" << file.utf8() << ": status=" << err.utf8() << oendl;
	return ret;
}

bool ZkbConfig::warning(const QXmlParseException& e) {
	QString tmp;

	tmp.sprintf("%d: warning: %s\n", e.lineNumber(), 
		(const char*) e.message().utf8());

	err += tmp;

	return true;
}

bool ZkbConfig::error(const QXmlParseException& e) {
	QString tmp;

	tmp.sprintf("%d: error: %s\n", e.lineNumber(), 
		(const char*) e.message().utf8());

	err += tmp;

	return true;
}

bool ZkbConfig::fatalError(const QXmlParseException& e) {
	QString tmp;

	tmp.sprintf("%d: fatal error: %s\n", e.lineNumber(), 
		(const char*) e.message().utf8());

	err += tmp;

	return false;
}

QString ZkbConfig::errorString() {
	return err;
}

// Implementation of ZkbHandler
ZkbHandler::ZkbHandler(ZkbConfig& z, Keymap& k, const QString& p):zkc(z), keymap(k), 
	prefix(p), ardelay(-1), arperiod(-1), currentState(0), currentAction(0) {
}

ZkbHandler::~ZkbHandler() {
}

bool ZkbHandler::startKeymapElement(int ard, int arp, const QString&) {
	ardelay = ard;
	arperiod = arp;

	return true;
}

bool ZkbHandler::startIncludeElement(const QString& file, 
	const QString& pref) {

	QString p = prefix;

	if (!pref.isNull()) {
		p += pref + ":";
	}
		

	bool ret = zkc.load(file, keymap, p);
	if (!ret) {
		setError("Error including file: " + file);
	}

	return ret;
}

bool ZkbHandler::startLabelElement(const QString& label, 
	const QString& state) {

	if (!keymap.addLabel(label, prefix + state)) {
		err = "label " + label + " already defined";
		return false;
	}

	return true;
}

bool ZkbHandler::startStateElement(const QString& name, 
	const QString& parentName, bool dflt) {

	currentStateName = prefix + name;
	currentState = keymap.getStateByName(currentStateName);

//	odebug << "state name=" << currentStateName.utf8() << "\n" << oendl; 

	State* parent = 0;
	if (!parentName.isEmpty()) {
		QString pn = prefix + parentName;
		parent = keymap.getStateByName(pn);
		if (parent == 0) {
			err = currentStateName + 
				": undefined parent state: " + pn;
			return false;
		}
	}

	if (currentState == 0) {
		currentState = new State(parent);
		keymap.addState(currentStateName, currentState);
	} else {
		if (parent!=0) {
			currentState->setParent(parent);
		}
	}

	if (dflt) {
		keymap.setCurrentState(currentState);
	}

	return true;
}

bool ZkbHandler::startMapElement(int keycode, bool pressed) {
	currentAction = currentState->get(keycode, pressed);
	if (currentAction == 0) {
		setError("keycode " + QString::number(keycode) + " not supported");
		return false;
	}

	currentAction->setEvent(false);
	currentAction->setState(0);

	return true;
}

bool ZkbHandler::startEventElement(int keycode, int unicode, int modifiers, 
	bool pressed, bool autorepeat) {

	currentAction->setEvent(true);
	currentAction->setKeycode(keycode);
	currentAction->setUnicode(unicode);
	currentAction->setModifiers(modifiers);
	currentAction->setPressed(pressed);
	currentAction->setAutorepeat(autorepeat);

	return true;
}

bool ZkbHandler::startNextStateElement(const QString& state) {
	State* s = keymap.getStateByName(prefix + state);
	if (s == 0) {
		setError("undefine state: " + prefix + state);
		return false;
	}

	currentAction->setState(s);
	return true;
}


bool ZkbHandler::endKeymapElement() {
	if (ardelay > 0) {
		keymap.setAutorepeatDelay(ardelay);
	}

	if (arperiod > 0) {
		keymap.setAutorepeatPeriod(arperiod);
	}
	
	return true;
}

bool ZkbHandler::endIncludeElement() {
	return true;
}

bool ZkbHandler::endLabelElement() {
	return true;
}

bool ZkbHandler::endStateElement() {
	currentState = 0;
	return true;
}

bool ZkbHandler::endMapElement() {
	currentAction = 0;
	return true;
}

bool ZkbHandler::endEventElement() {
	return true;
}

bool ZkbHandler::endNextStateElement() {
	return true;
}
