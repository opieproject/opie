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
	QString rot13(const QString &input);

protected slots:
	void slotCheck();
	void slotClicked();
	void slotSoundOff();
	void slotIMAPStatus(IMAPResponse &response);

private:
	Config *_config;
	QTimer *_intervalTimer;
	int _intervalMs;
	bool _zaurus, _ledOn;

};

#endif

