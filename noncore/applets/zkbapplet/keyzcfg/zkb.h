#ifndef ZKB_H
#define ZKB_H

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qwindowsystem_qws.h>
#include <qkeyboard_qws.h>
#include <qtimer.h>
#include <stdio.h>

class State;

class Action {
protected:
	State* state;
	ushort keycode;
	ushort unicode;
	int flags;

	enum {
		Shift_Mod = 1,
		Ctrl_Mod = 2,
		Alt_Mod = 4,
		Keypad_Mod = 8,
		Mod_Bits = 15,
		Press = 16,
		Autorepeat = 32,
		Event = 64,
		Defined = 128,
	};

	void setDefined(bool);

public:
	Action();
	Action(State*, ushort, ushort, int);
	~Action();

	State* getState() const;
	void setState(State*);

	bool hasEvent() const;
	void setEvent(bool);

	bool isDefined() const;

	int getKeycode() const;
	void setKeycode(int);

	int getUnicode() const;
	void setUnicode(int);

	int getModifiers() const;
	void setModifiers(int m);

	bool isPressed() const;
	void setPressed(bool);

	bool isAutorepeat() const;
	void setAutorepeat(bool);
};

class State {
protected:
	State* parent;
	Action* keys;

	enum {
		Key_a=0,
		Key_b=1,
		Key_c=2,
		Key_d=3,
		Key_e=4,
		Key_f=5,
		Key_g=6,
		Key_h=7,
		Key_i=8,
		Key_j=9,
		Key_k=10,
		Key_l=11,
		Key_m=12,
		Key_n=13,
		Key_o=14,
		Key_p=15,
		Key_q=16,
		Key_r=17,
		Key_s=18,
		Key_t=19,
		Key_u=20,
		Key_v=21,
		Key_w=22,
		Key_x=23,
		Key_y=24,
		Key_z=25,
		Key_Comma=26,
		Key_Slash=27,
		Key_Quote=28,
		Key_Dot=29,
		Key_Backspace=30,
		Key_Space=31,
		Key_Enter=32,
		Key_LeftShift=33,
		Key_RightShift=34,
		Key_Fn=35,
		Key_Tab=36,
		Key_Calendar=37,
		Key_Addressbook=38,
		Key_Home=39,
		Key_Menu=40,
		Key_Mail=41,
		Key_Cancel=42,
		Key_OK=43,
		Key_Left=44,
		Key_Up=45,
		Key_Right=46,
		Key_Down=47,
		Key_Middle=48,
		Key_Off=49,
		Key_Light=50,

		Key_Max=51
	};

	static const short x1[];
	static const short x2[];

	int translateKeycode(int keycode) const;

public:
	State(State* parent=0);
	State(const State&);
	~State();

	Action* get(int keycode, bool pressed, bool follow = false) const;
	bool set(int keycode, bool pressed, Action& action);

	State* getParent() const;
	void setParent(State*);
};

class Keymap : public QObject, public QWSServer::KeyboardFilter {
Q_OBJECT

public:
	Keymap();
	virtual ~Keymap();

	virtual bool filter(int unicode, int keycode, int modifiers, 
		bool isPress, bool autoRepeat);

	void enable();
	void disable();

	QStringList listStates();
	State* getStateByName(const QString& name);

	QStringList listLabels();
	State* getStateByLabel(const QString& label);

	bool addState(const QString& name, State* state);
	bool removeState(const QString& name, bool force = false);
	bool setCurrentState(State*);
	State* getCurrentState() const;
	QString getCurrentLabel();

	bool addLabel(const QString& label, const QString& state, 
		int index=-1);
	bool removeLabel(const QString& label);

	int getAutorepeatDelay() const;
	void setAutorepeatDelay(int);

	int getAutorepeatPeriod() const;
	void setAutorepeatPeriod(int);

signals:
	void stateChanged(const QString& name);

protected slots:
	void autoRepeat();

protected:
	QMap<QString, State*> states;
	QMap<QString, QString> labels;
	QStringList labelList;

	QMap<State*,QString> stateLabelMap;
	bool lsmapInSync;

	bool enabled;
	State* currentState;
	QString currentStateName;
	QString currentLabel;
	Action* autoRepeatAction;

	int repeatDelay;
	int repeatPeriod;
	QTimer repeater;

	QList<Action> findStateUsage(State* s);
	void generateLabelStateMaps();
};

#endif
