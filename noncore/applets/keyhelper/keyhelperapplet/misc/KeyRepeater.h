#ifndef _KEY_REPEATER_H_
#define _KEY_REPEATER_H_

#include <qobject.h>
#include <qtimer.h>
#include <qvaluelist.h>
#include <qwindowsystem_qws.h>

#include <syslog.h>

class KeyRepeater : public QObject
{
	Q_OBJECT
public:
	KeyRepeater();
	virtual ~KeyRepeater();

	typedef enum {
		DISABLE=0,
		ENABLE=1,
		KILL=2,
	} RepeaterMode;

	void start(int unicode, int keycode, int modifieres);
	void stop(int keycode = 0);
	void reset();

	void setRepeatable(int keycode, bool enable);
	bool isRepeatable(int keycode);

	void setMode(int mode){
		switch(mode){
		case DISABLE:
			m_mode = DISABLE;
			break;
		case ENABLE:
			m_mode = ENABLE;
			break;
		case KILL:
			m_mode = KILL;
			break;
		default:
			m_mode = ENABLE;
			break;
		}
	}
	RepeaterMode getMode(){
		return(m_mode);
	}

	void setDelay(int msec)
	{
		m_repeatdelay = msec;
	}
	void setPeriod(int msec)
	{
		m_repeatperiod = msec;
	}

	void statistics();
private slots:
	void autoRepeat();
private:
	int m_unicode;
	int m_keycode;
	int m_modifiers;

	int m_repeatdelay;
	int m_repeatperiod;

	QCString m_hookChannel;

	RepeaterMode m_mode;

	QValueList<int> m_disablekeys;

	QTimer* m_pTimer;

	void init();
	void clear();
signals:
	void keyEvent(int unicode, int keycode, int modifiers,
		bool isPress, bool autoRepeat);
};

#endif /* _KEY_REPEATER_H_ */
