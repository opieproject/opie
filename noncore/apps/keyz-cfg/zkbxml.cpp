#include "zkbxml.h"
#include "zkbnames.h"

static QString Keymap_Tag("keymap");
static QString Include_Tag("include");
static QString Label_Tag("label");
static QString State_Tag("state");
static QString Map_Tag("map");
static QString Event_Tag("event");
static QString NextState_Tag("next-state");

ZkbXmlHandler::ZkbXmlHandler() {
}

ZkbXmlHandler::~ZkbXmlHandler() {
}

bool ZkbXmlHandler::startElement(const QString&, const QString&, 
	const QString& name, const QXmlAttributes& attr) {

	bool ret = false;

	if (name == Keymap_Tag) {
		ret = start_keymap(attr);
	} else if (name == Include_Tag) {
		ret = start_include(attr);
	} else if (name == Label_Tag) {
		ret = start_label(attr);
	} else if (name == State_Tag) {
		ret = start_state(attr);
	} else if (name == Map_Tag) {
		ret = start_map(attr);
	} else if (name == Event_Tag) {
		ret = start_event(attr);
	} else if (name == NextState_Tag) {
		ret = start_next_state(attr);
	}

	elements.prepend(name);

	return ret;
}

bool ZkbXmlHandler::endElement(const QString&, const QString&, 
	const QString& name) {

	bool ret = false;

	elements.remove(elements.begin());

	if (name == Keymap_Tag) {
		ret = end_keymap();
	} else if (name == Include_Tag) {
		ret = end_include();
	} else if (name == Label_Tag) {
		ret = end_label();
	} else if (name == State_Tag) {
		ret = end_state();
	} else if (name == Map_Tag) {
		ret = end_map();
	} else if (name == Event_Tag) {
		ret = end_event();
	} else if (name == NextState_Tag) {
		ret = end_next_state();
	}

	return ret;
}

QString ZkbXmlHandler::errorString() {
	return err;
}

bool ZkbXmlHandler::startKeymapElement(int ardelay, int arperiod, const QString& author) {
	return false;
}

bool ZkbXmlHandler::startIncludeElement(const QString& file, 
	const QString& prefix) {

	return false;
}

bool ZkbXmlHandler::startLabelElement(const QString& label, 
	const QString& state) {

	return false;
}

bool ZkbXmlHandler::startStateElement(const QString& name, 
	const QString& parent, bool dflt) {

	return false;
}

bool ZkbXmlHandler::startMapElement(int keycode, bool pressed) {
	return false;
}

bool ZkbXmlHandler::startEventElement(int keycode, int unicode, int modifiers, 
	bool pressed, bool autorepeat) {

	return false;
}

bool ZkbXmlHandler::startNextStateElement(const QString& state) {
	return false;
}


bool ZkbXmlHandler::endKeymapElement() {
	return false;
}

bool ZkbXmlHandler::endIncludeElement() {
	return false;
}

bool ZkbXmlHandler::endLabelElement() {
	return false;
}

bool ZkbXmlHandler::endStateElement() {
	return false;
}

bool ZkbXmlHandler::endMapElement() {
	return false;
}

bool ZkbXmlHandler::endEventElement() {
	return false;
}

bool ZkbXmlHandler::endNextStateElement() {
	return false;
}


bool ZkbXmlHandler::start_keymap(const QXmlAttributes& attr) {
	int nattr = 0;
	int didx = attr.index("autorepeat-delay");
	int pidx = attr.index("autorepeat-period");
	int aidx = attr.index("author");
	int ard = -1;
	int arp = -1;
	QString author;

	if (!elements.isEmpty()) {
		setError("keymap element should be top-level element");
		return false;
	}

	if (didx >= 0) {
		QString s = attr.value(didx);
		bool ok;

		ard = s.toInt(&ok);
		if (!ok) {
			setError("Invalid autorepeat-delay value: " + s);
			return false;
		}

		nattr++;
	}

	if (pidx >= 0) {
		QString s = attr.value(pidx);
		bool ok;

		arp = s.toInt(&ok);
		if (!ok) {
			setError("Invalid autorepeat-period value: " + s);
			return false;
		}

		nattr++;
	}

	if (aidx >= 0) {
		author = attr.value(aidx);
		nattr++;
	}

	if (attr.length() > nattr) {
		setError("Unsupported attributes");
		return false;
	}

	return startKeymapElement(ard, arp, author);
}

bool ZkbXmlHandler::start_include(const QXmlAttributes& attr) {
	int nattr = 0;
	int fidx = attr.index("file");
	int pidx = attr.index("prefix");
	QString file;
	QString prefix((const char*) 0);

	if (elements.first() != Keymap_Tag) {
		setError("include element should be used only "
			"within keymap element");
		return false;
	}

	if (fidx >= 0) {
		file = attr.value(fidx);
		nattr++;
	} else {
		setError("Missing file attribute");
		return false;
	}

	if (pidx >= 0) {
		prefix = attr.value(pidx);
		nattr++;
	}

	if (attr.length() > nattr) {
		setError("Unsupported attributes");
		return false;
	}

	return startIncludeElement(file, prefix);
}

bool ZkbXmlHandler::start_label(const QXmlAttributes& attr) {
	int nattr = 0;
	int nidx = attr.index("name");
	int sidx = attr.index("state");
	QString name;
	QString state;

	if (elements.first() != Keymap_Tag) {
		setError("label element should be used only "
			"within keymap element");
		return false;
	}

	if (nidx >= 0) {
		name = attr.value(nidx);
		nattr++;
	} else {
		setError("Missing name attribute");
		return false;
	}

	if (sidx >= 0) {
		state = attr.value(sidx);
		nattr++;
	} else {
		setError("Missing name attribute");
		return false;
	}

	if (attr.length() > nattr) {
		setError("Unsupported attributes");
		return false;
	}

	return startLabelElement(name, state);
}

bool ZkbXmlHandler::start_state(const QXmlAttributes& attr) {
	int nattr = 0;
	int nidx = attr.index("name");
	int pidx = attr.index("parent");
	int didx = attr.index("default");
	QString name;
	QString parent((const char*) 0);
	bool dflt = false;

	if (elements.first() != Keymap_Tag) {
		setError("state element should be used only "
			"within keymap element");
		return false;
	}

	if (nidx >= 0) {
		name = attr.value(nidx);
		nattr++;
	} else {
		setError("Missing name attribute");
		return false;
	}

	if (pidx >= 0) {
		parent = attr.value(pidx);
		nattr++;
	}

	if (didx >= 0) {
		dflt = str2bool(attr.value(didx));
		if (!err.isEmpty()) {
			return false;
		}

		nattr++;
	}

	if (attr.length() > nattr) {
		setError("Unsupported attributes");
		return false;
	}

	return startStateElement(name, parent, dflt);
}

bool ZkbXmlHandler::start_map(const QXmlAttributes& attr) {
	int nattr = 0;
	int kidx = attr.index("keycode");
	int pidx = attr.index("pressed");
	int key;
	bool pressed;

	if (elements.first() != State_Tag) {
		setError("map element should be used only "
			"within state element");
		return false;
	}

	if (kidx >= 0) {
		key = str2key(attr.value(kidx));
		if (!err.isEmpty()) {
			return false;
		}
		nattr++;
	} else {
		setError("Missing keycode attribute");
		return false;
	}

	if (pidx >= 0) {
		pressed = str2bool(attr.value(pidx));
		if (!err.isEmpty()) {
			return false;
		}
		nattr++;
	} else {
		setError("Missing pressed attribute");
		return false;
	}

	if (attr.length() > nattr) {
		setError("Unsupported attributes");
		return false;
	}

	return startMapElement(key, pressed);
}

bool ZkbXmlHandler::start_event(const QXmlAttributes& attr) {
	int nattr = 0;
	int kidx = attr.index("keycode");
	int pidx = attr.index("pressed");
	int uidx = attr.index("unicode");
	int midx = attr.index("modifiers");
	int aidx = attr.index("autorepeat");

	int keycode;
	int unicode;
	int modifiers = 0;
	bool pressed;
	bool autorepeat = false;

	if (elements.first() != Map_Tag) {
		setError("event element should be used only "
			"within map element");
		return false;
	}

	if (kidx >= 0) {
		keycode = str2keycode(attr.value(kidx));
		if (!err.isEmpty()) {
			return false;
		}
		nattr++;
	} else {
		setError("Missing keycode attribute");
		return false;
	}

	if (uidx >= 0) {
		unicode = str2unicode(attr.value(uidx));
		if (!err.isEmpty()) {
			return false;
		}
		nattr++;
	} else {
		setError("Missing unicode attribute");
		return false;
	}

	if (midx >= 0) {
		modifiers = str2modifier(attr.value(midx));
		if (!err.isEmpty()) {
			return false;
		}
		nattr++;
	}

	if (pidx >= 0) {
		pressed = str2bool(attr.value(pidx));
		if (!err.isEmpty()) {
			return false;
		}
		nattr++;
	} else {
		setError("Missing pressed attribute");
		return false;
	}

	if (aidx >= 0) {
		autorepeat = str2bool(attr.value(aidx));
		if (!err.isEmpty()) {
			return false;
		}
		nattr++;
	}

	if (attr.length() > nattr) {
		setError("Unsupported attributes");
		return false;
	}

	return startEventElement(keycode, unicode, modifiers, pressed, 
		autorepeat);
}

bool ZkbXmlHandler::start_next_state(const QXmlAttributes& attr) {
	int nattr = 0;
	int nidx = attr.index("name");
	QString name;

	if (elements.first() != Map_Tag) {
		setError("next-state element should be used only "
			"within map element");
		return false;
	}

	if (nidx >= 0) {
		name = attr.value(nidx);
		nattr++;
	} else {
		setError("Missing name attribute");
		return false;
	}

	if (attr.length() > nattr) {
		setError("Unsupported attributes");
		return false;
	}

	return startNextStateElement(name);
}

bool ZkbXmlHandler::end_keymap() {
	return endKeymapElement();
}

bool ZkbXmlHandler::end_include() {
	return endIncludeElement();
}

bool ZkbXmlHandler::end_label() {
	return endLabelElement();
}

bool ZkbXmlHandler::end_state() {
	return endStateElement();
}

bool ZkbXmlHandler::end_map() {
	return endMapElement();
}

bool ZkbXmlHandler::end_event() {
	return endEventElement();
}

bool ZkbXmlHandler::end_next_state() {
	return endNextStateElement();
}

void ZkbXmlHandler::setError(const QString& e) {
	err = e;
}

int ZkbXmlHandler::str2key(const QString& s) {
	int ret;

#ifdef USE_ZKB_NAMES
	ret = KeyNames::find(s);
	if (ret == -1) {
		setError("Invalid value: " + s);
	}

#else 
	ret = str2uint(s);
#endif

	return ret;
}

int ZkbXmlHandler::str2modifier(const QString& val) {
	int ret;

#ifdef USE_ZKB_NAMES
	int n, i;
	ret = 0;
	n = 0;
	do {
		i = val.find('|', n);
		if (i < 0) {
			i = val.length();
		}

		QString s = val.mid(n, i - n);
		int v = ModifierNames::find(s.stripWhiteSpace());

		if (v == -1) {
			setError("Invalid value: " + val);
			return -1;
		}

		ret |= v;
		n = i + 1;
	} while (n < val.length());
#else 
	ret = str2uint(val);
#endif

	return ret;
}

bool ZkbXmlHandler::str2bool(const QString& s) {
	if (s == "true") {
		return true;
	} else {
		return false;
	}
}

int ZkbXmlHandler::str2unicode(const QString& s) {
	return str2uint(s);
}

int ZkbXmlHandler::str2keycode(const QString& s) {
	int ret;

#ifdef USE_ZKB_NAMES
	ret = KeycodeNames::find(s);
	if (ret == -1) {
		setError("Invalid value: " + s);
	}

#else
	ret = str2uint(s);
#endif

	return ret;
}

int ZkbXmlHandler::str2uint(const QString& s) {
	int ret;
	bool ok;
	QString val = s;
	int r;

	if (val.left(2) == "0x") {
		val = s.mid(2);
		r = 16;
	} else if (val.left(1) == "0") {
		val = s.mid(1);
		r = 8;
	} else {
		r = 10;
	}

	ret = val.toInt(&ok, r);
	if (!ok) {
		setError("Invalid value: " + s);
		ret = -1;
	}

	return ret;
}
