#ifndef DIALER_H
#define DIALER_H

#include <qdialog.h>

#include "profile.h"

class QLabel;
class QProgressBar;

class Dialer : public QDialog
{
	Q_OBJECT
	public:
		Dialer(const Profile& profile, int fd, QWidget *parent = NULL, const char *name = NULL);
		~Dialer();

	public slots:
		void slotCancel();
		void slotAutostart();

	private:
		void switchState(int newstate);
		void reset();
		void dial(const QString& number);
		void trydial(const QString& number);

		void send(const QString& msg);
		QString receive();

		enum States
		{
			state_cancel,
			state_preinit,
			state_init,
			state_options,
			state_dialtone,
			state_dialing,
			state_online
		};

		QLabel *status;
		QProgressBar *progress;
		QPushButton *cancel;
		int state;
		int usercancel;
		const Profile& m_profile;
		int m_fd;
};

#endif

