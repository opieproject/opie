#ifndef VIEWMAIL_H
#define VIEWMAIL_H

#include <qlistview.h>
#include <qmap.h>
#include <qstringlist.h>

#include "viewmailbase.h"
#include "mailtypes.h"

class AttachItemStore 
{
public:
	void setMimeType(QString mimeType) { _mimeType = mimeType; }
	QString mimeType() { return _mimeType; }
	void setFileName(QString fileName) { _fileName = fileName; }
	QString fileName() { return _fileName; }
	void setDescription(QString description) { _description = description; }
	QString description() { return _description; }
	void setPartNum(QString partNum) { _partNum = partNum; }
	QString partNum() { return _partNum; }

private:
	QString _mimeType, _fileName, _description, _partNum;

};

class AttachItem : public QListViewItem
{
public:
	AttachItem(QListView *parent, AttachItemStore &attachment);
	AttachItem(QListViewItem *parent, AttachItemStore &attachment);

	AttachItemStore attachItemStore() { return _attachItemStore; }

private:
	AttachItemStore _attachItemStore;

};

class ViewMail : public ViewMailBase
{
	Q_OBJECT

public:
	ViewMail(  QWidget *parent = 0, const char *name = 0, WFlags fl = Qt::WType_Modal);
	~ViewMail();

	void hide();
	void exec();
        void setMail( RecMail mail );
        void setBody( RecBody body );

protected:
	QString deHtml(const QString &string);

protected slots:
	void slotReply();
	void slotForward();
        void setText();

private:
	bool _inLoop;
	QString m_mailHtml;
	bool m_gotBody;

        // 0 from  1 subject  2 bodytext 3 date
        QMap <int,QString>  m_mail;
        // 0 to 1 cc 2 bcc
        QMap <int,QStringList> m_mail2;

};

#endif
