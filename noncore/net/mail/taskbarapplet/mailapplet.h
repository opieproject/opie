#ifndef MAILAPPLET_H
#define MAILAPPLET_H

#include <qbutton.h>

#include <libmailwrapper/statusmail.h>

class Config;
class QTimer;

class MailApplet : public QWidget {

	Q_OBJECT

public:
	MailApplet( QWidget *parent = 0 );
        ~MailApplet();

protected:
	void gotNewMail();
        void mouseReleaseEvent( QMouseEvent* );
        void paintEvent( QPaintEvent* );

protected slots:
        void startup();
	void slotCheck();
	void slotClicked();
private:
	Config *m_config;
	QTimer *m_intervalTimer;
	int m_intervalMs;
        StatusMail* m_statusMail;
        int m_newMails;

};

#endif

