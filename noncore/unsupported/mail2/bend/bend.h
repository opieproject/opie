#ifndef BEND_H
#define BEND_H

#include <qbutton.h>

class Config;
class QTimer;
class IMAPResponse;

class BenD : public QButton
{
	Q_OBJECT

public:
	BenD(QWidget *parent = 0, const char  *name = 0, WFlags fl = 0);

protected:
	void drawButton(QPainter *);
	void drawButtonText(QPainter *);
	void gotNewMail();

protected slots:
	void slotCheck();
	void slotClicked();
	void slotIMAPStatus(IMAPResponse &response);

private:
	Config *_config;
	QTimer *_intervalTimer;
	int _intervalMs;

};

#endif

