#include <qmultilineedit.h>
#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qaction.h>
#include <qlabel.h>
#include <qvbox.h>

#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>

#include "mailstatusbar.h"
#include "listviewplus.h"
#include "composerbase.h"

ComposerBase::ComposerBase(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl)
{
	setCaption(tr("Compose Message"));
	setToolBarsMovable(false);

	toolbar = new QPEToolBar(this);
	menubar = new QPEMenuBar( toolbar );
	mailmenu = new QPopupMenu( menubar );
	menubar->insertItem( tr( "Mail" ), mailmenu );
	addToolBar(toolbar);
	toolbar->setHorizontalStretchable(true);

	QLabel *spacer = new QLabel(toolbar);
	spacer->setBackgroundMode(QWidget::PaletteButton);
	toolbar->setStretchableWidget(spacer);

	sendmail = new QAction(tr("Send the mail"), QIconSet(Resource::loadPixmap("mail/sendmail")), 0, 0, this);
	sendmail->addTo(toolbar);
	sendmail->addTo(mailmenu);

	queuemail = new QAction(tr("Queue the mail"), QIconSet(Resource::loadPixmap("mail/sendall")), 0, 0, this);
	queuemail->addTo(toolbar);
	queuemail->addTo(mailmenu);

	attachfile = new QAction(tr("Attach a file"), QIconSet(Resource::loadPixmap("mail/attach")), 0, 0, this, 0, true);
	attachfile->addTo(toolbar);
	attachfile->addTo(mailmenu);
	connect(attachfile, SIGNAL(toggled(bool)), SLOT(slotAttachfileChanged(bool)));

	addressbook = new QAction(tr("Addressbook"), QIconSet(Resource::loadPixmap("mail/addbook")), 0, 0, this);
	addressbook->addTo(toolbar);
	addressbook->addTo(mailmenu);

	QWidget *main = new QWidget(this);
	setCentralWidget(main);

	QGridLayout *layout = new QGridLayout(main);

	fromBox = new QComboBox(main);
	fromBox->insertItem(tr("From"), POPUP_FROM_FROM);
	fromBox->insertItem(tr("Reply"), POPUP_FROM_REPLYTO);
	layout->addWidget(fromBox, 0, 0);

	connect(fromBox, SIGNAL(activated(int)), SLOT(slotFromMenuChanged(int)));

	QHBoxLayout *fromLayout = new QHBoxLayout();
	layout->addLayout(fromLayout, 0, 1);

	from = new QComboBox(main);
	fromLayout->addWidget(from);

	replyto = new QLineEdit(main);
	replyto->hide();
	fromLayout->addWidget(replyto);

	receiversBox = new QComboBox(main);
	receiversBox->insertItem(tr("To"), POPUP_RECV_TO);
	receiversBox->insertItem(tr("Cc"), POPUP_RECV_CC);
	receiversBox->insertItem(tr("Bcc"), POPUP_RECV_BCC);
	layout->addWidget(receiversBox, 1, 0);

	connect(receiversBox, SIGNAL(activated(int)), SLOT(slotReceiverMenuChanged(int)));

	QHBoxLayout *receiverLayout = new QHBoxLayout();
	layout->addLayout(receiverLayout, 1, 1);

	to = new QLineEdit(main);
	receiverLayout->addWidget(to);

	cc = new QLineEdit(main);
	cc->hide();
	receiverLayout->addWidget(cc);

	bcc = new QLineEdit(main);
	bcc->hide();
	receiverLayout->addWidget(bcc);

	subjectBox = new QComboBox(main);
	subjectBox->insertItem(tr("Subj."), POPUP_SUBJ_SUBJECT);
	subjectBox->insertItem(tr("Prio."), POPUP_SUBJ_PRIORITY);
	layout->addWidget(subjectBox, 2, 0);
	connect(subjectBox, SIGNAL(activated(int)), SLOT(slotSubjectMenuChanged(int)));

	QHBoxLayout *subjectLayout = new QHBoxLayout();
	layout->addLayout(subjectLayout, 2, 1);

	subject = new QLineEdit(main);
	subjectLayout->addWidget(subject);

	priority = new QComboBox(main);
	priority->insertItem(tr("Low"), POPUP_PRIO_LOW);
	priority->insertItem(tr("Normal"), POPUP_PRIO_NORMAL);
	priority->insertItem(tr("High"), POPUP_PRIO_HIGH);
	priority->setCurrentItem(POPUP_PRIO_NORMAL);
	priority->hide();
	subjectLayout->addWidget(priority);

	QVBox *view = new QVBox(main);
	layout->addMultiCellWidget(view, 3, 3, 0, 1);

	message = new QMultiLineEdit(view);
	message->setMinimumHeight(30);

	attachWindow = new QMainWindow(view, 0, 0);
	attachWindow->setMinimumHeight(80);
	attachWindow->setMaximumHeight(80);
	attachWindow->setToolBarsMovable(false);
	attachWindow->hide();

	attachToolbar = new QPEToolBar(attachWindow);
	attachToolbar->setVerticalStretchable(true);
	
	addattach = new QAction(tr("Add an Attachement"), QIconSet(Resource::loadPixmap("mail/newmail")), 0, 0, this);
	addattach->addTo(attachToolbar);

	delattach = new QAction(tr("Remove Attachement"), QIconSet(Resource::loadPixmap("mail/delete")), 0, 0, this);
	delattach->addTo(attachToolbar);

	QLabel *attachSpacer = new QLabel(attachToolbar);
	attachSpacer->setBackgroundMode(QWidget::PaletteButton);
	attachToolbar->setStretchableWidget(attachSpacer);

	attachWindow->addToolBar(attachToolbar, QMainWindow::Left);

	attachView = new ListViewPlus(attachWindow);
	attachView->addColumn(tr("Name"), 80);
	attachView->addColumn(tr("Description"), 110);
	attachView->setAllColumnsShowFocus(true);
	attachWindow->setCentralWidget(attachView);

	attachPopup = new QPopupMenu(attachView);
	attachPopup->insertItem(tr("Rename"), POPUP_ATTACH_RENAME);
	attachPopup->insertItem(tr("Change Description"), POPUP_ATTACH_DESC);
	attachPopup->insertSeparator();
	attachPopup->insertItem(tr("Remove"), POPUP_ATTACH_REMOVE);
	attachView->setPopup(attachPopup);

	status = new MailStatusBar(view);
}

void ComposerBase::slotAttachfileChanged(bool toggled)
{
	if (toggled) {
		if (attachWindow->isHidden()) attachWindow->show();
	} else {
		if (!attachWindow->isHidden()) attachWindow->hide();
	}
}

void ComposerBase::slotFromMenuChanged(int id)
{
	if (POPUP_FROM_FROM == id) {
		if (from->isHidden()) from->show();
		if (!replyto->isHidden()) replyto->hide();
	} else if (POPUP_FROM_REPLYTO == id) {
		if (!from->isHidden()) from->hide();
		if (replyto->isHidden()) replyto->show();
	}
}

void ComposerBase::slotReceiverMenuChanged(int id)
{
	if (POPUP_RECV_TO == id) {
		if (to->isHidden()) to->show();
		if (!cc->isHidden()) cc->hide();
		if (!bcc->isHidden()) bcc->hide();
	} else if (POPUP_RECV_CC == id) {
		if (!to->isHidden()) to->hide();
		if (cc->isHidden()) cc->show();
		if (!bcc->isHidden()) bcc->hide();
	} else if (POPUP_RECV_BCC == id) {
		if (!to->isHidden()) to->hide();
		if (!cc->isHidden()) cc->hide();
		if (bcc->isHidden()) bcc->show();
	}
}

void ComposerBase::slotSubjectMenuChanged(int id)
{
	if (POPUP_SUBJ_SUBJECT == id) {
		if (subject->isHidden()) subject->show();
		if (!priority->isHidden()) priority->hide();
	} else if (POPUP_SUBJ_PRIORITY == id) {
		if (!subject->isHidden()) subject->hide();
		if (priority->isHidden()) priority->show();
	}
}
