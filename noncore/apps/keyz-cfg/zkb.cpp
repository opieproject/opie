#include "zkb.h"
#include <stdio.h>

// Implementation of Action class
Action::Action():state(0), keycode(0), unicode(0), flags(0) {
}

Action::Action(State* s, ushort kc, ushort uni, int f):
	state(s), keycode(kc), unicode(uni), flags(f) {
}

Action::~Action() {
}

State* Action::getState() const {
	return state;
}

void Action::setState(State* s) {
	state = s;
	setDefined(true);
}

bool Action::hasEvent() const {
	return flags & Event;
}

void Action::setEvent(bool e) {
	flags = (flags & ~Event) | ((e) ? Event : 0);

	if (e) {
		setDefined(true);
	} else {
		if (state == 0) {
			setDefined(false);
		}
	}
}

bool Action::isDefined() const {
	return flags & Defined;
}

void Action::setDefined(bool d) {
	flags = (flags & ~Defined) | ((d) ? Defined : 0);
}

int Action::getKeycode() const {
	return keycode;
}

void Action::setKeycode(int c) {
	keycode = (ushort) c;
	setEvent(true);
}

int Action::getUnicode() const {
	return unicode;
}

void Action::setUnicode(int u) {
	unicode = (ushort) u;
	setEvent(true);
}

int Action::getModifiers() const {
	int ret = 0;
	if (flags & Shift_Mod) {
		ret |= Qt::ShiftButton;
	}

	if (flags & Ctrl_Mod) {
		ret |= Qt::ControlButton;
	}

	if (flags & Alt_Mod) {
		ret |= Qt::AltButton;
	}

	if (flags & Keypad_Mod) {
		ret |= Qt::Keypad;
	}

	return ret;
}

void Action::setModifiers(int m) {
	int n = 0;

	if (m & Qt::ShiftButton) {
		n |= Shift_Mod;
	}

	if (m & Qt::ControlButton) {
		n |= Ctrl_Mod;
	}

	if (m & Qt::AltButton) {
		n |= Alt_Mod;
	}

	if (m & Qt::Keypad) {
		n |= Keypad_Mod;
	}

	flags = flags & ~Mod_Bits | n;
	setEvent(true);
}

bool Action::isPressed() const {
	return (flags & Press) != 0;
}

void Action::setPressed(bool p) {
	flags = (flags & ~Press) | ((p) ? Press : 0);
	setEvent(true);
}

bool Action::isAutorepeat() const {
	return (flags & Autorepeat) != 0;
}

void Action::setAutorepeat(bool p) {
	flags = (flags & ~Autorepeat) | ((p) ? Autorepeat : 0);
	setEvent(true);
}

// Implementation of State class
const short State::x1[] = { /* from 0x20 to 0x5f */
	31,  0, 28,  3,  5,  6,  9, 28, 	/* 0x20 - 0x27 */
	11, 26, 10, 13, 26,  1, 29, 27,		/* 0x28 - 0x2f */
	15, 16, 22,  4, 17, 19, 24, 20,		/* 0x30 - 0x37 */
	 8, 14, 29, 26, 29, 12, 32, 27,		/* 0x38 - 0x3f */
	18,  0,  1,  2,  3,  4,  5,  6,		/* 0x40 - 0x47 */
	 7,  8,  9, 10, 11, 12, 13, 14,		/* 0x48 - 0x4f */
	15, 16, 17, 18, 19, 20, 21, 22,		/* 0x50 - 0x57 */
	23, 24, 25, 30, -1, 26, 28,  7,		/* 0x58 - 0x5f */
	31, -1, -1, -1, -1, -1, -1, -1,		/* 0x60 - 0x67 */
	-1, -1, -1, -1, -1, -1, -1, -1,		/* 0x68 - 0x6f */
	-1, -1, -1, -1, -1, -1, -1, -1,		/* 0x70 - 0x77 */
	-1, -1, -1, 29, 31, 32, 32, 28,		/* 0x78 - 0x7f */
};

const short State::x2[] = { /* from 0x1000 to 0x1057*/
	42, 36, -1, 30, 32, -1, -1, -1,		/* 0x1000 - 0x1007 */
	-1, -1, -1, -1, -1, -1, -1, -1,		/* 0x1008 - 0x100f */
	-1, -1, 44, 45, 46, 47, -1, -1,		/* 0x1010 - 0x1017 */
	-1, -1, -1, -1, -1, -1, -1, -1,		/* 0x1018 - 0x101f */
	33, 35, 34, -1, 36, 27, -1, -1,		/* 0x1020 - 0x1027 */
	-1, -1, -1, -1, -1, -1, -1, -1,		/* 0x1028 - 0x102f */
	-1, -1, -1, -1, -1, -1, -1, -1,		/* 0x1030 - 0x1037 */
	37, 38, 40, 39, 41, -1, -1, -1,		/* 0x1038 - 0x103f */
	-1, -1, -1, -1, -1, 35, -1, -1,		/* 0x1040 - 0x1047 */
	-1, -1, -1, -1, -1, 48, -1, -1,		/* 0x1048 - 0x104f */
	43, 49, 50, -1, -1, -1, -1, -1,		/* 0x1050 - 0x1057 */
};
	
State::State(State* p):parent(p), keys(0) {
	keys = new Action[Key_Max * 2 + 1];
}

State::State(const State& s) {
	parent = s.parent;
	keys = new Action[Key_Max * 2 + 1];
	memcpy(keys, s.keys, sizeof(Action) * (Key_Max * 2 + 1));
}

State::~State() {
	if (keys!=0) {
		delete [] keys;
	}
}

Action* State::get(int keycode, bool pressed, bool follow) const {
	Action* ret = 0;
	int n = translateKeycode(keycode);

	if (n != -1 && keys != 0) {
		if (pressed) {
			n += Key_Max;
		}
		ret = &keys[n];
	}

	if (ret==0 || !ret->isDefined()) {
		if (follow && parent!=0) {
			ret = parent->get(keycode, pressed, follow);
		}
	}

	return ret;
}

bool State::set(int keycode, bool pressed, Action& action) {
	int n = translateKeycode(keycode);

	if (n==-1 || keys==0) {
		return false;
	}

	if (pressed) {
		n += Key_Max + 1;
	}

	keys[n] = action;
	return true;
}

State* State::getParent() const {
	return parent;
}

void State::setParent(State* s) {
	parent = s;
}

int State::translateKeycode(int keycode) const {
	if (keycode < 0x20) {
		return -1;
	}

	if (keycode < 0x80) {
		return x1[keycode - 0x20];
	}

	if (keycode < 0x1000) {
		return -1;
	}

	if (keycode < 0x1057) {
		return x2[keycode - 0x1000];
	}

	return -1;
}

// Implementation of Keymap class
Keymap::Keymap():enabled(true), currentState(0), autoRepeatAction(0), repeater(this) {
	repeatDelay=400;
	repeatPeriod=80;
	connect(&repeater, SIGNAL(timeout()), this, SLOT(autoRepeat()));
}

Keymap::~Keymap() {
	QMap<QString, State*>::Iterator it;
	for(it = states.begin(); it != states.end(); ++it) {
		delete it.data();
	}
	states.clear();
}

bool Keymap::filter(int unicode, int keycode, int modifiers,
	bool isPress, bool autoRepeat) {

	qDebug("filter: >>> unicode=%x, keycode=%x, modifiers=%x, "
		"ispressed=%x\n", unicode, keycode, modifiers, isPress);

	if (!enabled) {
		return false;
	}

	// the second check is workaround to make suspend work if
	// the user pressed it right after he did resume. for some
	// reason the event sent by qt has autoRepeat true in this 
	// case
	if (autoRepeat && keycode != 4177) {
		return true;
	}

	(void) unicode; (void) modifiers;

	Action* action = currentState->get(keycode, isPress, true);
	if (action==0 || !action->isDefined()) {
		return true;
	}

	if (action->hasEvent()) {
		qDebug("filter:<<< unicode=%x, keycode=%x, modifiers=%x, "
			"ispressed=%x\n", action->getUnicode(), 
			action->getKeycode(), action->getModifiers(),
			action->isPressed());

		QWSServer::sendKeyEvent(action->getUnicode(),
			action->getKeycode(), action->getModifiers(),
			action->isPressed(), false);
	}

	if (action->isAutorepeat()) {
		autoRepeatAction = action;
		repeater.start(repeatDelay, TRUE);
	} else {
		autoRepeatAction = 0;
	}

	State* nstate = action->getState();
	if (nstate != 0) {
		setCurrentState(nstate);
		QString lbl = getCurrentLabel();
		if (!lbl.isEmpty()) {
			emit stateChanged(lbl);
		}
	}


	return true;
}

void Keymap::enable() {
	enabled = true;
}

void Keymap::disable() {
	enabled = false;
}

QStringList Keymap::listStates() {
	QStringList ret;

	QMap<QString, State*>::Iterator it;
	for(it = states.begin(); it != states.end(); ++it) {
		ret.append(it.key());
	}

	return ret;
}

State* Keymap::getStateByName(const QString& name) {
	QMap<QString, State*>::Iterator it = states.find(name);

	if (it == states.end()) {
		return 0;
	}

	return it.data();
}

QStringList Keymap::listLabels() {
	QStringList ret;

	for(uint i = 0; i < labelList.count(); i++) {
		ret.append(*labelList.at(i));
	}

	return ret;
}

State* Keymap::getStateByLabel(const QString& label) {
	QMap<QString, QString>::Iterator lit = labels.find(label);
	State* state = 0;

	if (lit == labels.end()) {
		return 0;
	}

	QString name = lit.data();

	int n = name.find(":*");
	if (n>=0 && n==(int)(name.length()-2)) {
		name=name.left(name.length() - 1);

		n = currentStateName.findRev(":");
		if (n >= 0) {
			name += currentStateName.mid(n+1);
		}
	}

//	odebug << "look for: " << (const char*) name.utf8() << "\n" << oendl; 
	QMap<QString, State*>::Iterator sit = states.find(name);
	if (sit != states.end()) {
		state = sit.data();
	}

	return state;
}

bool Keymap::addState(const QString& name, State* state) {
	if (states.find(name) != states.end()) {
		return false;
	}

	states.insert(name, state);
	lsmapInSync = false;

	if (currentState == 0) {
		setCurrentState(state);
	}

	return true;
}

State* Keymap::getCurrentState() const {
	return currentState;
}

QString Keymap::getCurrentLabel() {
	return currentLabel;
}

bool Keymap::setCurrentState(State* state) {
	QMap<QString, State*>::Iterator it;
	for(it = states.begin(); it != states.end(); ++it) {
		State* s = it.data();
		if (s == state) {
			currentState = s;
			currentStateName = it.key();

			qDebug("state changed: %s\n", (const char*) 
				currentStateName.utf8());

			if (!lsmapInSync) {
				generateLabelStateMaps();
			}

			QMap<State*, QString>::Iterator tit;
			tit = stateLabelMap.find(state);
			if (tit != stateLabelMap.end()) {
				currentLabel = tit.data();
			} else {
//				odebug << "no label for: " + currentStateName + "\n" << oendl; 
				currentLabel = "";
			}

			return true;
		}
	}

	return false;
}

bool Keymap::removeState(const QString& name, bool force) {
	QMap<QString, State*>::Iterator it = states.find(name);

	if (it == states.end()) {
		return false;
	}

	State* state = it.data();
	QList<Action> acts = findStateUsage(state);

	if (!acts.isEmpty()) {
		if (!force) {
			return false;
		} else {
			for(Action* a = acts.first(); a != 0; a = acts.next()) {
				a->setState(0);
			}
		}
	}

	if (state == currentState) {
		if (states.begin() != states.end()) {
			setCurrentState(states.begin().data());
		}
	}

	states.remove(it);
	delete state;

	lsmapInSync = false;

	return true;
}

void Keymap::autoRepeat() {
	if (autoRepeatAction != 0) {
		qDebug("filter:<<< unicode=%x, keycode=%x, modifiers=%x, "
			"ispressed=%x\n", autoRepeatAction->getUnicode(),
			autoRepeatAction->getKeycode(),
			autoRepeatAction->getModifiers(),
			autoRepeatAction->isPressed());

		QWSServer::sendKeyEvent(autoRepeatAction->getUnicode(),
			autoRepeatAction->getKeycode(), 
			autoRepeatAction->getModifiers(),
			autoRepeatAction->isPressed(), true);
	}

	repeater.start(repeatPeriod, TRUE);
}

bool Keymap::addLabel(const QString& label, const QString& state, int index) {
	if (labels.find(label) != labels.end()) {
		return false;
	}

	labels.insert(label, state);
	const QString& l = labels.find(label).key();
	if (index == -1) {
		labelList.append(l);
	} else {
		labelList.insert(labelList.at(index), l);
	}

	lsmapInSync = false;

	return true;
}

bool Keymap::removeLabel(const QString& label) {

	if (labels.find(label) == labels.end()) {
		return false;
	}

	labels.remove(label);
	labelList.remove(label);
	lsmapInSync = false;

	if (label == currentLabel) {
		currentLabel = "";
	}

	return true;
}

int Keymap::getAutorepeatDelay() const {
	return repeatDelay;
}

void Keymap::setAutorepeatDelay(int n) {
	repeatDelay = n;
}

int Keymap::getAutorepeatPeriod() const {
	return repeatPeriod;
}

void Keymap::setAutorepeatPeriod(int n) {
	repeatPeriod = n;
}

QList<Action> Keymap::findStateUsage(State* s) {
	QList<Action> ret;

	QMap<QString, State*>::Iterator it;
	for(it = states.begin(); it != states.end(); ++it) {
		State* state = it.data();

		for(int i = 0; i < 0x1100; i++) {
			Action* action = state->get(i, false);
			if (action!=0 && action->getState()==s) {
				ret.append(action);
			}

			action = state->get(i, true);
			if (action!=0 && action->getState()==s) {
				ret.append(action);
			}
		}
	}

	return ret;
}

void Keymap::generateLabelStateMaps() {
	stateLabelMap.clear();

	QMap<QString, QString>::Iterator lit;
	for(lit = labels.begin(); lit != labels.end(); ++lit) {
		QString label = lit.key();
		QString name = lit.data();

		bool wc = false;
		int n = name.find("*");
		if (n>=0 && n==(int)(name.length()-1)) {
			name=name.left(name.length() - 1);
			wc = true;
		}

		QMap<QString, State*>::Iterator sit;
		for(sit = states.begin(); sit != states.end(); ++sit) {
			QString sname = sit.key();
			State* state = sit.data();

			if (sname.length() < name.length()) {
				continue;
			}

			if (sname.left(name.length()) == name) {
				if (wc || sname.length()==name.length()) {
					stateLabelMap.insert(state, label);
				}
					
			}
		}
	}

	lsmapInSync = true;
}
