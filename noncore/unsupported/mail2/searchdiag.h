#ifndef SEARCHDIAG_H
#define SEARCHDIAG_H

#include "searchdiagbase.h"
#include "mailtable.h"

class IMAPResponse;

class SearchDiag : public SearchDiagBase
{
	Q_OBJECT

public:
	SearchDiag(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected slots:
	void accept();
	void slotInItemActivated(int index);
	void folderSelected(Folder folder);
	void slotMailClicked(IMAPResponseFETCH response, IMAPHandler *);

	void slotIMAPSelect(IMAPResponse &);
	void slotIMAPSearch(IMAPResponse &);
	void slotIMAPFetch(IMAPResponse &);

private:
	bool _selected;
	Folder _folder;

};

#endif
