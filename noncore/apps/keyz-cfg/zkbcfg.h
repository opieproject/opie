#ifndef ZKBCFG_H
#define ZKBCFG_H

#include <qxml.h>
#include "zkb.h"
#include "zkbxml.h"

class ZkbConfig : public QXmlErrorHandler {
public:
	ZkbConfig(const QString& dir);
	virtual ~ZkbConfig();

	bool load(const QString& file, Keymap& keymap, const QString& prefix);

	virtual bool warning(const QXmlParseException& e);
	virtual bool error(const QXmlParseException& e);
	virtual bool fatalError(const QXmlParseException& e);
	virtual QString errorString();

protected:
	QString path;
	QMap<QString, int> includedFiles;
	QString err;
};

class ZkbHandler : public ZkbXmlHandler {
public:
	ZkbHandler(ZkbConfig& zkc, Keymap& keymap, const QString& prefix);
	virtual ~ZkbHandler();

protected:
	ZkbConfig& zkc;
	Keymap& keymap;
	QString prefix;

	// stuff for keymap tag
	int ardelay;
	int arperiod;

	// stuff for state tag
	QString currentStateName;
	State* currentState;

	// stuff for map tag
	Action* currentAction;

	virtual bool startKeymapElement(int ardelay, int arperiod, 
		const QString& author);
	virtual bool startIncludeElement(const QString& file, 
		const QString& prfix);
	virtual bool startLabelElement(const QString& label, 
		const QString& state);
	virtual bool startStateElement(const QString& name, 
		const QString& parent, bool dflt);
	virtual bool startMapElement(int key, bool pressed);
	virtual bool startEventElement(int keycode, int unicode, int modifiers,
		bool pressed, bool autorepeat);
	virtual bool startNextStateElement(const QString& state);

	virtual bool endKeymapElement();
	virtual bool endIncludeElement();
	virtual bool endLabelElement();
	virtual bool endStateElement();
	virtual bool endMapElement();
	virtual bool endEventElement();
	virtual bool endNextStateElement();
};

#endif
