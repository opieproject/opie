#ifndef VIEWMAIL_H
#define VIEWMAIL_H

#include <qlistview.h>
#include <qmap.h>
#include <qstringlist.h>

#include "viewmailbase.h"
#include "mailtypes.h"

class AttachItem : public QListViewItem
{
public:
    AttachItem(QListView * parent,QListViewItem *after, const QString&mime,const QString&file,const QString&desc,int num);
    int Partnumber() { return _partNum; }

private:
    int _partNum;
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
        void slotItemClicked( QListViewItem * item , const QPoint & point, int c );

private:
	bool _inLoop;
	QString m_mailHtml;
	bool m_gotBody;
        RecBody m_body;

        // 0 from  1 subject  2 bodytext 3 date
        QMap <int,QString>  m_mail;
        // 0 to 1 cc 2 bcc
        QMap <int,QStringList> m_mail2;

};

#endif
