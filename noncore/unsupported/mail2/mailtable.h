#ifndef MAILTABLE_H
#define MAILTABLE_H

#include <qdatetime.h>

#include "listviewplus.h"
#include "imapresponse.h"
#include "folderwidget.h"
#include "imaphandler.h"

class MailPart
{
public:
	void setMimeType(QString mimeType) { _mimeType = mimeType; }
	void setFileName(QString fileName) { _fileName = fileName; }
	void setDescription(QString description) { _description = description; }
	void setData(QString data) { _data = data; }

	QString mimeType() { return _mimeType; }
	QString fileName() { return _fileName; }
	QString description() { return _description; }
	QString data() { return _data; }

private:
	QString _mimeType, _fileName, _description, _data;

};

class Mail
{
public:
	enum ReadState { New, Unread, Old };

	void setFrom(QString from) { _from = from; }
	void setTo(QString to) { _to = to; }
	void setSubject(QString subject) { _subject = subject; }
	void setDate(QDate date) { _date = date; }
	void setBody(QString body) { _body = body; }
	void setReadState(ReadState readState) { _readState = readState; }
	void setMailParts(QValueList<MailPart> parts) { _parts = parts; }
	void addMailParts(MailPart part) { _parts.append(part); }

	QString from() { return _from; }
	QString to() { return _to; }
	QString subject() { return _subject; }
	QDate date() { return _date; }
	QString body() { return _body; }
	ReadState readState() { return _readState; }
	QValueList<MailPart> mailParts() { return _parts; }

private:
	QString _from, _to, _subject, _body;
	QDate _date;
	QValueList<MailPart> _parts;
	ReadState _readState;	 

};

class MailTableItem : public QListViewItem
{
public:
	MailTableItem(QListView *parent, IMAPResponseFETCH fetch);

	IMAPResponseFETCH fetch() { return _fetch; }

protected:
	void paintCell(QPainter *painter, const QColorGroup &cg, int col, int width, int align);

private:
	IMAPResponseFETCH _fetch;
	bool _draft, _new, _unseen;

};

class MailTable : public ListViewPlus
{
	Q_OBJECT

public:
	MailTable(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

	void addMail(Mail &mail);
	void setHeaders(QValueList<IMAPResponseFETCH> response);

public slots:
	void stop() { _stopped = true; }
	void setFolder(Folder folder);

signals:
	void totalSteps(int steps);
	void progress(int progress);
	void resetProgress();
	void mailClicked(IMAPResponseFETCH mail, IMAPHandler *handler);
	void status(const QString &text);
	void stopEnabled(bool stop);

protected:
	static const int MENU_COPY = 0;
	static const int MENU_MARK_READ = 0;
	static const int MENU_MARK_UNREAD = 1;
	static const int MENU_MARK_MARKED = 2;
	static const int MENU_MARK_UNMARKED = 3;
	static const int MENU_MARK = 1;
	static const int MENU_DELETE = 2;

protected slots:
	void itemClicked(QListViewItem *);

	void slotIMAPSelect(IMAPResponse &response);
	void slotIMAPFetch(IMAPResponse &response);

private:
	QWidget *_parent;
	IMAPHandler *_handler;
	int _downloadSteps, _lastStep, _currentProgress;
	bool _stopped;

};

#endif

