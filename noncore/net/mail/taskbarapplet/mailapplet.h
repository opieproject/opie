#ifndef MAILAPPLET_H
#define MAILAPPLET_H

#include <qbutton.h>

#include <libmailwrapper/statusmail.h>

class Config;
class QTimer;

class MailApplet : public QButton {

	Q_OBJECT

public:
	MailApplet( QWidget *parent = 0, const char  *name = 0, WFlags fl = 0 );
        ~MailApplet();

protected:
	void drawButton(QPainter *);
	void drawButtonText(QPainter *);
	void gotNewMail();

protected slots:
        void startup();
	void slotCheck();
	void slotClicked();
private:
	Config *m_config;
	QTimer *m_intervalTimer;
	int m_intervalMs;
        StatusMail* m_statusMail;

};

#endif

