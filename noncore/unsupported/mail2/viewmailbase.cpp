#include <qtextbrowser.h>
#include <qlistview.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qlabel.h>
#include <qvbox.h>

#include <qpe/resource.h>

#include "viewmailbase.h"
#include "opendiag.h"

ViewMailBase::ViewMailBase(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl)
{
	setCaption(tr("E-Mail by %1"));
	setToolBarsMovable(false);

	toolbar = new QToolBar(this);
	toolbar->setHorizontalStretchable(true);
	addToolBar(toolbar);

	reply = new QAction(tr("Reply"), QIconSet(Resource::loadPixmap("mail/reply")), 0, 0, this);
	reply->addTo(toolbar);

	forward = new QAction(tr("Forward"), QIconSet(Resource::loadPixmap("mail/forward")), 0, 0, this);
	forward->addTo(toolbar);

	attachbutton = new QAction(tr("Attachments"), QIconSet(Resource::loadPixmap("mail/attach")), 0, 0, this, 0, true);
	attachbutton->addTo(toolbar);
	connect(attachbutton, SIGNAL(toggled(bool)), SLOT(slotChangeAttachview(bool)));

	deleteMail = new QAction(tr("Delete Mail"), QIconSet(Resource::loadPixmap("mail/delete")), 0, 0, this);
	deleteMail->addTo(toolbar);

	QLabel *spacer = new QLabel(toolbar);
	spacer->setBackgroundMode(QWidget::PaletteButton);
	toolbar->setStretchableWidget(spacer);

	QVBox *view = new QVBox(this);
	setCentralWidget(view);

	attachments = new QListView(view);
	attachments->setMinimumHeight(90);
	attachments->setMaximumHeight(90);
	attachments->setAllColumnsShowFocus(true);
	attachments->addColumn("Mime Type", 100);
	attachments->addColumn("Filename", 100);
	attachments->addColumn("Description", 100);
	attachments->hide();

	browser = new QTextBrowser(view);

	openDiag = new OpenDiag(view);
	openDiag->hide();

}

void ViewMailBase::slotChangeAttachview(bool state)
{
	if (state) attachments->show();
	else attachments->hide();
}


