#ifndef DEFINE_CONSTANTS_H
#define DEFINE_CONSTANTS_H

#include <qpe/resource.h>

#define USER_AGENT          "OpieMail v0.3"

#define PIC_COMPOSEMAIL     "mail/composemail"
#define PIC_SENDQUEUED      "mail/sendqueued"
#define PIC_SHOWFOLDERS     "mail/showfolders"
#define PIC_SYNC            "mail/sync"
#define PIC_IMAPFOLDER      "mail/imapfolder"
#define PIC_MBOXFOLDER      "mail/mboxfolder"
#define PIC_POP3FOLDER      "mail/pop3folder"
#define PIC_INBOXFOLDER     "mail/inbox"
#define PIC_OUTBOXFOLDER    "mail/outbox"
#define PIC_LOCALFOLDER     "mail/localfolder"

#define PIC_OFFLINE         "mail/notconnected"

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
#define PIXMAP_MBOXFOLDER   QPixmap( Resource::loadPixmap( PIC_MBOXFOLDER ) )
#define PIXMAP_OUTBOXFOLDER QPixmap( Resource::loadPixmap( PIC_OUTBOXFOLDER) )
#define PIXMAP_LOCALFOLDER  QPixmap( Resource::loadPixmap( PIC_LOCALFOLDER) )
#define PIXMAP_OFFLINE      QPixmap( Resource::loadPixmap( PIC_OFFLINE) )

#define IMAP_PORT           "143"
#define IMAP_SSL_PORT       "993"
#define SMTP_PORT           "25"
#define SMTP_SSL_PORT       "465"
#define POP3_PORT           "110"
#define POP3_SSL_PORT       "995"
#define NNTP_PORT           "119"
#define NNTP_SSL_PORT       "563"

/* used for decoding imapfoldername */
#define UNDEFINED 64
#define MAXLINE  76
#define UTF16MASK       0x03FFUL
#define UTF16SHIFT      10
#define UTF16BASE       0x10000UL
#define UTF16HIGHSTART  0xD800UL
#define UTF16HIGHEND    0xDBFFUL
#define UTF16LOSTART    0xDC00UL
#define UTF16LOEND      0xDFFFUL

#endif
