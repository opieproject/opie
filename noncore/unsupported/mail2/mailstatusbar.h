#ifndef MAILSTATUSBAR_H
#define MAILSTATUSBAR_H

#include <qhbox.h>

class QLabel;
class QToolButton;
class QProgressBar;

class MailStatusBar : public QHBox
{
	Q_OBJECT

public:
	MailStatusBar(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

public slots:
	void setStatusText(const QString &text);
	void setStopEnabled(bool enabled);
	void setProgressTotalSteps(int steps);
	void setProgress(int progress);
	void resetProgress();

signals:
	void stop();

protected slots:
	void stopClicked();

private:
	QLabel *label;
	QToolButton *stopButton;
	QProgressBar *progress;

};

#endif

