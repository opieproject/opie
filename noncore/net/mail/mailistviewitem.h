#ifndef __MAILLISTVIEWITEM_H
#define __MAILLISTVIEWITEM_H

#include <qlistview.h>
#include <libmailwrapper/mailtypes.h>


class MailListViewItem:public QListViewItem
{
public:
    MailListViewItem(QListView * parent, MailListViewItem * after );
    virtual ~MailListViewItem(){}
    
    void storeData(const RecMail&data);
    const RecMail&data()const;
    void showEntry();
    
protected:
    RecMail mail_data;
};

#endif
