#ifndef COMPOSER_H
#define COMPOSER_H

#include <qlistview.h>
#include <qpe/applnk.h>

#include "mailfactory.h"
#include "composerbase.h"
#include "configfile.h"

class AttachViewItem : public QListViewItem
{
public:
	AttachViewItem(QListView *parent, Attachment &attachment);

	Attachment attachment() { return _attachment; }

private:
	Attachment _attachment;

};

class Composer : public ComposerBase
{
	Q_OBJECT

public:
	Composer(QWidget *parent = 0, const char *name = 0, WFlags fl = Qt::WType_Modal);
	~Composer();

	void hide();
	void exec();

	void setSendMail(SendMail &sendMail);

protected slots:
	void slotPopupHandler(int itemid);
	void slotSendMail();
	void slotSendError(const QString &);
	void slotSendFinished();
	void slotResizing();
	void slotFillStuff();
	void slotFromChanged(int id);
	void slotOpenAddressPicker();
	void slotAddAttach();
	void slotDelAttach();

protected:
	QValueList<Account> accountsLoaded;

private:
	bool _inLoop;
	QString _inReplyTo;

};

#endif
