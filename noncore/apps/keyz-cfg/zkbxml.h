#ifndef ZKBXML_H
#define ZKBXML_H

#include <qxml.h>

class ZkbXmlHandler : public QXmlDefaultHandler {
public:
	ZkbXmlHandler();
	virtual ~ZkbXmlHandler();

	virtual bool startElement(const QString&, const QString&, 
		const QString& name, const QXmlAttributes& attr);

	virtual bool endElement(const QString&, const QString&, 
		const QString& name);

	virtual QString errorString();

protected:
	QString err;
	QStringList elements;

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

	bool start_keymap(const QXmlAttributes&);
	bool start_include(const QXmlAttributes&);
	bool start_label(const QXmlAttributes&);
	bool start_state(const QXmlAttributes&);
	bool start_map(const QXmlAttributes&);
	bool start_event(const QXmlAttributes&);
	bool start_next_state(const QXmlAttributes&);
	bool end_keymap();
	bool end_include();
	bool end_label();
	bool end_state();
	bool end_map();
	bool end_event();
	bool end_next_state();

	void setError(const QString&);
	int str2key(const QString&);
	int str2modifier(const QString&);
	bool str2bool(const QString&);
	int str2unicode(const QString&);
	int str2keycode(const QString&);
	int str2uint(const QString&);
};

#endif
