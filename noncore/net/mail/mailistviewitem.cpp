#include "mailistviewitem.h"
#include <qtextstream.h>
#include <qpe/resource.h>

MailListViewItem::MailListViewItem(QListView * parent, MailListViewItem * item )
        :QListViewItem(parent,item),mail_data()
{
}

void MailListViewItem::showEntry()
{
    if ( mail_data.getFlags().testBit( FLAG_ANSWERED ) == true) {
        setPixmap( 0, Resource::loadPixmap( "mail/kmmsgreplied") );
    } else if ( mail_data.getFlags().testBit( FLAG_SEEN ) == true )  {
        /* I think it looks nicer if there are not such a log of icons but only on mails
           replied or new - Alwin*/
        //setPixmap( 0, Resource::loadPixmap( "mail/kmmsgunseen") );
    } else  {
        setPixmap( 0, Resource::loadPixmap( "mail/kmmsgnew") );
    }
    double s = mail_data.Msgsize();
    int w;
    w=0;

    while (s>1024) {
        s/=1024;
        ++w;
        if (w>=2) break;
    }

    QString q="";
    QString fsize="";
    switch(w) {
    case 1:
        q="k";
        break;
    case 2:
        q="M";
        break;
    default:
        break;
    }

    {
        QTextOStream o(&fsize);
        if (w>0) o.precision(2); else o.precision(0);
        o.setf(QTextStream::fixed);
        o << s << " " << q << "Byte";
    }

    setText(1,mail_data.getSubject());
    setText(2,mail_data.getFrom());
    setText(3,fsize);
    setText(4,mail_data.getDate());
}

void MailListViewItem::storeData(const RecMail&data)
{
    mail_data = data;
}

const RecMail& MailListViewItem::data()const
{
    return mail_data;
}
