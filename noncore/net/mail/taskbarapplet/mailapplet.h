#ifndef MAILAPPLET_H
#define MAILAPPLET_H

#include <qbutton.h>

class Config;
class QTimer;

class MailApplet : public QButton {

	Q_OBJECT

public:
	MailApplet(QWidget *parent = 0, const char  *name = 0, WFlags fl = 0);

protected:
	void drawButton(QPainter *);
	void drawButtonText(QPainter *);
	void gotNewMail();

protected slots:
	void slotCheck();
	void slotClicked();
private:
	Config *m_config;
	QTimer *m_intervalTimer;
	int m_intervalMs;

};

#endif

