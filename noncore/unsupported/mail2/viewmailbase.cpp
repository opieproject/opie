#include <qtextbrowser.h>
#include <qlistview.h>
#include <qaction.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qpopupmenu.h>

#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>

#include "viewmailbase.h"
#include "opendiag.h"

ViewMailBase::ViewMailBase(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl)
{
	setCaption(tr("E-Mail by %1"));
	setToolBarsMovable(false);

	toolbar = new QPEToolBar(this);
  menubar = new QPEMenuBar( toolbar );
  mailmenu = new QPopupMenu( menubar );
  menubar->insertItem( tr( "Mail" ), mailmenu );

	toolbar->setHorizontalStretchable(true);
	addToolBar(toolbar);

	QLabel *spacer = new QLabel(toolbar);
	spacer->setBackgroundMode(QWidget::PaletteButton);
	toolbar->setStretchableWidget(spacer);

	reply = new QAction(tr("Reply"), QIconSet(Resource::loadPixmap("mail/reply")), 0, 0, this);
	reply->addTo(toolbar);
 	reply->addTo(mailmenu);

	forward = new QAction(tr("Forward"), QIconSet(Resource::loadPixmap("mail/forward")), 0, 0, this);
	forward->addTo(toolbar);
	forward->addTo(mailmenu);

	attachbutton = new QAction(tr("Attachments"), QIconSet(Resource::loadPixmap("mail/attach")), 0, 0, this, 0, true);
	attachbutton->addTo(toolbar);
	attachbutton->addTo(mailmenu);
	connect(attachbutton, SIGNAL(toggled(bool)), SLOT(slotChangeAttachview(bool)));

	deleteMail = new QAction(tr("Delete Mail"), QIconSet(Resource::loadPixmap("mail/delete")), 0, 0, this);
	deleteMail->addTo(toolbar);
	deleteMail->addTo(mailmenu);

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


