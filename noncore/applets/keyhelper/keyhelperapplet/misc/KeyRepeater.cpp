#include "KeyRepeater.h"

KeyRepeater::KeyRepeater()
{
	qDebug("KeyRepeater::KeyRepeater()");
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(autoRepeat()));
	init();
}

KeyRepeater::~KeyRepeater()
{
	qDebug("KeyRepeater::~KeyRepeater()");
	delete m_pTimer;
}

void KeyRepeater::start(int unicode, int keycode, int modifiers)
{
	m_unicode = unicode;
	m_keycode = keycode;
	m_modifiers = modifiers;
	if(m_mode == ENABLE){
		m_pTimer->stop();
		if(isRepeatable(keycode)){
			/* repeater start */
			m_pTimer->start(m_repeatdelay, TRUE);
		}
	}
}

void KeyRepeater::stop(int keycode)
{
	if(keycode == 0
		|| keycode == m_keycode
		|| isRepeatable(keycode) == false){
		m_pTimer->stop();
	}
}

void KeyRepeater::init()
{
	m_mode = ENABLE;
	m_repeatdelay = 400;
	m_repeatperiod = 60;
	m_disablekeys.append(0);
	m_disablekeys.append(Qt::Key_Escape);
	m_disablekeys.append(Qt::Key_Shift);
	m_disablekeys.append(Qt::Key_Control);
	m_disablekeys.append(Qt::Key_Alt);
	m_disablekeys.append(Qt::Key_Meta);
	for(int i=Qt::Key_F1; i<=Qt::Key_F35; i++){
		m_disablekeys.append(i);
	}
}

void KeyRepeater::clear()
{
	m_disablekeys.clear();
}

void KeyRepeater::reset()
{
	clear();
	init();
}

void KeyRepeater::setRepeatable(int keycode, bool enable)
{
	if(enable){
		QValueList<int>::Iterator it = m_disablekeys.find(keycode);
		if(it != m_disablekeys.end()){
			m_disablekeys.remove(it);
		}
	} else {
		if(m_disablekeys.contains(keycode) == false){
			m_disablekeys.append(keycode);
		}
	}
}

bool KeyRepeater::isRepeatable(int keycode)
{
	if(m_disablekeys.contains(keycode)){
		return(false);
	} else {
		return(true);
	}
}

void KeyRepeater::autoRepeat()
{
	/* key release event */
#if 0
	sendKeyEvent(
		m_unicode,
		m_keycode,
		m_modifiers,
		FALSE,
		TRUE);
	/* key press event */
	sendKeyEvent(
		m_unicode,
		m_keycode,
		m_modifiers,
		TRUE,
		TRUE);
#else
	emit keyEvent(
		m_unicode,
		m_keycode,
		m_modifiers,
		FALSE,
		TRUE);
	/* key press event */
	emit keyEvent(
		m_unicode,
		m_keycode,
		m_modifiers,
		TRUE,
		TRUE);
#endif
	/* start auto repeat */
	m_pTimer->start(m_repeatperiod);
#if 0
	qDebug("autoRepeat[%x][%x][%x]",
		m_unicode,
		m_keycode,
		m_modifiers);
#endif
}

void KeyRepeater::statistics()
{
	qWarning("KeyRepeater::statistics()");
	qWarning(" delay[%d] period[%d]", m_repeatdelay, m_repeatperiod);
	for(QValueList<int>::Iterator it=m_disablekeys.begin();
			it!=m_disablekeys.end(); ++it){
		qDebug(" disable[%x]", *it);
	}
}
