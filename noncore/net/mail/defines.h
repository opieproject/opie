#ifndef DEFINE_CONSTANTS_H
#define DEFINE_CONSTANTS_H

#include <qpe/resource.h>

#define USER_AGENT          "OpieMail v0.0.1-alpha"

#define PIC_COMPOSEMAIL     "opiemail/composemail"
#define PIC_SENDQUEUED      "opiemail/sendqueued"
#define PIC_SHOWFOLDERS     "opiemail/showfolders"
#define PIC_SEARCHMAILS     "opiemail/searchmails"
#define PIC_EDITSETTINGS    "opiemail/editsettings"
#define PIC_EDITACCOUNTS    "opiemail/editaccounts"
#define PIC_SYNC            "opiemail/sync"
#define PIC_IMAPFOLDER      "opiemail/imapfolder"
#define PIC_POP3FOLDER      "opiemail/pop3folder"
#define PIC_INBOXFOLDER     "opiemail/inbox"

#define ICON_COMPOSEMAIL    QIconSet( Resource::loadPixmap( PIC_COMPOSEMAIL ) ) 
#define ICON_SENDQUEUED     QIconSet( Resource::loadPixmap( PIC_SENDQUEUED ) ) 
#define ICON_SHOWFOLDERS    QIconSet( Resource::loadPixmap( PIC_SHOWFOLDERS ) ) 
#define ICON_SEARCHMAILS    QIconSet( Resource::loadPixmap( PIC_SEARCHMAILS ) ) 
#define ICON_EDITSETTINGS   QIconSet( Resource::loadPixmap( PIC_EDITSETTINGS ) ) 
#define ICON_EDITACCOUNTS   QIconSet( Resource::loadPixmap( PIC_EDITACCOUNTS ) ) 
#define ICON_SYNC           QIconSet( Resource::loadPixmap( PIC_SYNC ) ) 

#define PIXMAP_IMAPFOLDER   QPixmap( Resource::loadPixmap( PIC_IMAPFOLDER ) )
#define PIXMAP_POP3FOLDER   QPixmap( Resource::loadPixmap( PIC_POP3FOLDER ) )
#define PIXMAP_INBOXFOLDER  QPixmap( Resource::loadPixmap( PIC_INBOXFOLDER) )

#define IMAP_PORT           "143"
#define IMAP_SSL_PORT       "993"
#define SMTP_PORT           "25"
#define SMTP_SSL_PORT       "465"
#define POP3_PORT           "110"
#define POP3_SSL_PORT       "995"
#define NNTP_PORT           "119"
#define NNTP_SSL_PORT       "563"

#endif
