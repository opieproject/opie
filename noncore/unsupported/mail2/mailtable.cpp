#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qheader.h>
#include <qtimer.h>

#include "folderwidget.h"
#include "imaphandler.h"
#include "mailtable.h"

MailTableItem::MailTableItem(QListView *parent, IMAPResponseFETCH fetch)
	: QListViewItem(parent), _fetch(fetch)
{
	_draft = false;
	_new = false;
	_unseen = true;

	QValueList<IMAPResponseEnums::IMAPResponseFlags>::Iterator it;
	QValueList<IMAPResponseEnums::IMAPResponseFlags> flags = fetch.flags();
	for (it = flags.begin(); it != flags.end(); it++) {
		if (*it == IMAPResponseEnums::Draft) _draft = true;
		else if (*it == IMAPResponseEnums::Recent) _new = true;
		else if (*it == IMAPResponseEnums::Seen) _unseen = false; 
	}

	setText(0, fetch.envelope().from()[0].name().isEmpty() ? QObject::tr("(no from)") : fetch.envelope().from()[0].name());
	setText(1, fetch.envelope().subject().isEmpty() ? QObject::tr("(no subject)") : fetch.envelope().subject());
	setText(2, fetch.envelope().mailDate().isEmpty() ? QObject::tr("(no date)") : fetch.envelope().mailDate());
}

void MailTableItem::paintCell(QPainter *painter, const QColorGroup &cg, int col, int width, int align)
{
	QColor color = cg.color(QColorGroup::Text);
	QFont font = painter->font();
	if (_draft) {
		color = QColor("#707070");
	} else if (_new) {
		color = QColor("#ff0000");
		font.setBold(true);
	} else if (_unseen) {
		font.setBold(true);
	}

	painter->setFont(font);
	QColorGroup cg_(cg);
	cg_.setColor(QColorGroup::Text, color);

	QListViewItem::paintCell(painter, cg_, col, width, align);
}

MailTable::MailTable(QWidget *parent, const char *name, WFlags fl)
	: ListViewPlus(parent, name, fl), _parent(parent)
{
	_stopped = false;

	setSorting(-1);

	setAllColumnsShowFocus(true);
	addColumn(tr("From"), 100);
	addColumn(tr("Subject"), 100);
	addColumn(tr("Date"), 100);

	QPopupMenu *menu = new QPopupMenu(this);
	menu->insertItem(tr("Copy"), MENU_COPY);

	QPopupMenu *markMenu = new QPopupMenu(this);
	markMenu->insertItem(tr("Seen"), MENU_MARK_READ);
	markMenu->insertItem(tr("Unseen"), MENU_MARK_UNREAD);
	markMenu->insertSeparator();
	markMenu->insertItem(tr("Marked"), MENU_MARK_MARKED);
	markMenu->insertItem(tr("Unmarked"), MENU_MARK_UNMARKED);

	menu->insertItem(tr("Mark as..."), markMenu, MENU_MARK);
	menu->insertSeparator();
	menu->insertItem(tr("Delete Mail"), MENU_DELETE);
	setPopup(menu);

	connect(this, SIGNAL(clicked(QListViewItem *)), SLOT(itemClicked(QListViewItem *)));
}

void MailTable::setFolder(Folder folder)
{
	folder.topFolder().handler()->iSelect(folder.fullName());
	_handler = folder.topFolder().handler();
	connect(folder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPSelect(IMAPResponse &)));
}

void MailTable::setHeaders(QValueList<IMAPResponseFETCH> response)
{
	clear();
	QValueList<IMAPResponseFETCH>::Iterator it;
	for (it = response.begin(); it != response.end(); it++) {
		(void) new MailTableItem(this, *it);
	}
}

void MailTable::slotIMAPSelect(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPSelect(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		clear();
		QString mails = response.EXISTS()[0].mails();
		if (mails.toInt() == 0) {
			emit status(tr("Mailbox contained no mails."));
			return;
		}
		int a = mails.toInt() / 5;
		int b = mails.toInt() % 5;

		_downloadSteps = a;
		if (b > 0) _downloadSteps++;
		_lastStep = b;
		_currentProgress = 0;

		emit totalSteps(_downloadSteps);
		emit progress(_currentProgress);
		emit stopEnabled(true);

		response.imapHandler()->iFetch(QString("1:%1").arg((a == 0) ? b : 5), "ENVELOPE FLAGS UID");
		emit status(tr("Getting mail headers..."));
		connect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPFetch(IMAPResponse &)));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>An error occoured during the selection of the mailbox. (Server said: %1)</p>").arg(response.statusResponse().comment()), tr("Ok"));
	}
}

void MailTable::slotIMAPFetch(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPFetch(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		QValueList<IMAPResponseFETCH>::Iterator it;
		QValueList<IMAPResponseFETCH> fetch = response.FETCH();
		for (it = fetch.begin(); it != fetch.end(); it++) {
			(void) new MailTableItem(this, *it);
		}
		emit progress(++_currentProgress);

		if (_currentProgress != _downloadSteps) {
			if (_stopped) {
				_currentProgress = 0;
				_downloadSteps = 0;
				_lastStep = 0;
				_stopped = false;
				emit status(tr("Stopped"));
				emit resetProgress();
				emit stopEnabled(false);
			} else {
				response.imapHandler()->iFetch(QString("%1:%2").arg(_currentProgress * 5 + 1).arg((_currentProgress + 1 == _downloadSteps) ? _currentProgress * 5 + _lastStep : _currentProgress * 5 + 5), "ENVELOPE FLAGS UID");
				connect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPFetch(IMAPResponse &)));
			}
		} else {
			_currentProgress = 0;
			_downloadSteps = 0;
			_lastStep = 0;
			emit status(tr("Got all mail headers."));
			emit resetProgress();
			emit stopEnabled(false);
		}
	} else {
		emit status(tr("<font color=#ff0000>Couldn't fetch mail."));
	}
}

void MailTable::itemClicked(QListViewItem *item)
{
	if (item == NULL) return;
	emit mailClicked(((MailTableItem *)item)->fetch(), _handler);
}


