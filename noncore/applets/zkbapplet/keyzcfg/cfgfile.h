#ifndef CFGFILE_H
#define CFGFILE_H

#include <qlist.h>
#include "zkbxml.h"

class CfgEntry {
public:
	CfgEntry();	
	CfgEntry(const QString& file, const QString& label);

	const QString& getFile() const;
	void setFile(const QString& f);
	const QString& getLabel() const;
	void setLabel(const QString& l);

protected:
	QString file;
	QString label;
};

class CfgFile {
public:
	CfgFile();
	~CfgFile();

	QList<CfgEntry>& getEntries();
	bool replaceEntry(const QString& file, const QString& label, 
		int index = -1);
	bool deleteEntry(const QString& file);

	int getAutorepeatDelay() const;
	void setAutorepeatDelay(int);
	int getAutorepeatPeriod() const;
	void setAutorepeatPeriod(int);

protected:
	QList<CfgEntry> entries;
	int ardelay;
	int arperiod;
};

class CfgParser : public QXmlErrorHandler {
public:
	CfgParser();
	virtual ~CfgParser();

	bool load(QString file, CfgFile& cfg);
	bool save(QString file, CfgFile& cfg);

	void addLabel(const QString& name, const QString& state);
	void addFile(const QString& file, const QString& prefix);

	int getAutorepeatDelay() const;
	void setAutorepeatDelay(int);
	int getAutorepeatPeriod() const;
	void setAutorepeatPeriod(int);

	virtual bool warning(const QXmlParseException& e);
	virtual bool error(const QXmlParseException& e);
	virtual bool fatalError(const QXmlParseException& e);
	virtual QString errorString();

	QString getError();

protected:
	QString err;
	QMap<QString, QString> labels;
	QMap<QString, QString> includes;
	QList<QString> labelList;
	QList<QString> includeList;
	int ardelay;
	int arperiod;
};

class CfgHandler : public ZkbXmlHandler {
public:
	CfgHandler(CfgParser &);
	virtual ~CfgHandler();

protected:
	CfgParser& cfg;

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
