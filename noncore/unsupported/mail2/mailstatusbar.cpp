#include <qprogressbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>

#include "mailstatusbar.h"

MailStatusBar::MailStatusBar(QWidget *parent, const char *name, WFlags fl)
	: QHBox(parent, name, fl)
{
	label = new QLabel(this);
	QFont tmpFont = label->font();
	tmpFont.setPixelSize(8);
	label->setFont(tmpFont);

	stopButton = new QToolButton(this);
	stopButton->setText(" X ");
	stopButton->setMaximumHeight(15);
	stopButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	connect(stopButton, SIGNAL(clicked()), SLOT(stopClicked()));

	progress = new QProgressBar(this);
	progress->setCenterIndicator(true);
	progress->setMinimumHeight(15);
	progress->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	progress->setFrameShape(QFrame::Box);
	progress->setFrameShadow(QFrame::Raised);
}

void MailStatusBar::setStatusText(const QString &text)
{
	label->setText(text);
}

void MailStatusBar::setStopEnabled(bool enabled)
{
	stopButton->setEnabled(enabled);
}

void MailStatusBar::setProgressTotalSteps(int steps)
{
	progress->setTotalSteps(steps);
}

void MailStatusBar::setProgress(int pg)
{
	progress->setProgress(pg);
}

void MailStatusBar::resetProgress()
{
	progress->reset();
}

void MailStatusBar::stopClicked()
{
	emit stop();
}

