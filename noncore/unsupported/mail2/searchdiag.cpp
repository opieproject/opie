#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>

#include <qpe/qpeapplication.h>

#include "imaphandler.h"
#include "searchdiag.h"
#include "viewmail.h"

#define INMENU_BODY	0
#define INMENU_HEADERF	1
#define INMENU_SUBJECT	2
#define INMENU_FROM	3
#define INMENU_TO	4

SearchDiag::SearchDiag(QWidget *parent, const char *name, WFlags fl)
	: SearchDiagBase(parent, name,  fl)
{	
	_selected = false;

	in->insertItem(tr("Body"), INMENU_BODY);
	in->insertItem(tr("Header Field"), INMENU_HEADERF);
	in->insertItem(tr("Subject"), INMENU_SUBJECT);
	in->insertItem(tr("From"), INMENU_FROM);
	in->insertItem(tr("To"), INMENU_TO);

	connect(folderView, SIGNAL(folderSelected(Folder)), SLOT(folderSelected(Folder)));
	connect(in, SIGNAL(activated(int)), SLOT(slotInItemActivated(int)));
	connect(mailTable, SIGNAL(mailClicked(IMAPResponseFETCH, IMAPHandler *)), SLOT(slotMailClicked(IMAPResponseFETCH, IMAPHandler *)));
}

void SearchDiag::accept()
{
	if (searchFor->text().isEmpty()) {
		QMessageBox::information(this, tr("Error"), tr("<p>Please enter what to search for.</p>"), tr("Ok"));
		return;
	}

	if (!_selected) {
		QMessageBox::information(this, tr("Error"), tr("<p>Please select a folder.</p>"), tr("Ok"));
		return;
	}

	if (in->currentItem() == INMENU_HEADERF && other->currentText().isEmpty()) {
		QMessageBox::information(this, tr("Error"), tr("<p>Please enter a header field to search in.</p>"), tr("Ok"));
		return;
	}

	_folder.topFolder().handler()->iSelect(_folder.fullName());
	connect(_folder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPSelect(IMAPResponse &)));
}

void SearchDiag::folderSelected(Folder folder)
{
	_selected = true;
	_folder = folder;
}

void SearchDiag::slotIMAPSelect(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPSelect(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		if (in->currentItem() == INMENU_BODY) {
			response.imapHandler()->iSearch("BODY \"" + searchFor->text() + "\"");
		} else if (in->currentItem() == INMENU_HEADERF) {
			response.imapHandler()->iSearch("HEADER \""+ other->currentText() + "\" \"" + searchFor->text() + "\"");
		} else if (in->currentItem() == INMENU_SUBJECT) {
			response.imapHandler()->iSearch("SUBJECT \"" + searchFor->text() + "\"");
		} else if (in->currentItem() == INMENU_FROM) {
			response.imapHandler()->iSearch("FROM \"" + searchFor->text() + "\"");
		} else if (in->currentItem() == INMENU_TO) {
			response.imapHandler()->iSearch("TO \"" + searchFor->text() + "\"");
		} else return;

		connect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPSearch(IMAPResponse &)));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>Could not select the folder. Aborting. (Server said: %1)").arg(response.statusResponse().comment()), tr("Ok"));
	}
}

void SearchDiag::slotIMAPSearch(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPSearch(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		IMAPResponseSEARCH results = response.SEARCH()[0];
		if (results.mails().count() == 0) {
			QMessageBox::information(this, tr("Results"), tr("<p>No mails match your criteria.</p>"), tr("Ok"));
			return;
		}

		response.imapHandler()->iFetch(results.mails().join(","), "ENVELOPE FLAGS UID");
		connect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPFetch(IMAPResponse &)));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>Search failed. (Server said: %1)").arg(response.statusResponse().comment()), tr("Ok"));
	}
}

void SearchDiag::slotIMAPFetch(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPSearch(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		mailTable->setHeaders(response.FETCH());
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>Couldn't fetch the mail headers. (Server said: %1)").arg(response.statusResponse().comment()));
	}
}

void SearchDiag::slotMailClicked(IMAPResponseFETCH fetch, IMAPHandler *)
{
	ViewMail *viewMail = new ViewMail(fetch, _folder.topFolder().handler());
	viewMail->showMaximized();
	viewMail->show();
}

void SearchDiag::slotInItemActivated(int index)
{
	if (index == INMENU_HEADERF) {
		other->setEnabled(true);
	} else {
		other->setEnabled(false);
	}
}

