#ifndef DIALER_H
#define DIALER_H

#include <qdialog.h>

class QLabel;
class QProgressBar;

class Dialer : public QDialog
{
	Q_OBJECT
	public:
		Dialer(const QString& number, QWidget *parent = NULL, const char *name = NULL);
		~Dialer();

	public slots:
		void slotCancel();

	private:
		void switchState(int newstate);
		void reset();
		void dial(const QString& number);

		void send(const QString& msg);
		QString receive();

		enum States
		{
			state_init,
			state_options,
			state_dialing,
			state_online
		};

		QLabel *status;
		QProgressBar *progress;
		QPushButton *cancel;
		int state;
};

#endif

