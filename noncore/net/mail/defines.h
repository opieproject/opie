#ifndef DEFINE_CONSTANTS_H
#define DEFINE_CONSTANTS_H

#include <opie2/oresource.h>
using namespace Opie::Core;

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
#define PIC_DOCUMENT        "DocsIcon"

#define ICON_COMPOSEMAIL    QIconSet( OResource::loadPixmap( PIC_COMPOSEMAIL, OResource::SmallIcon ) )
#define ICON_SENDQUEUED     QIconSet( OResource::loadPixmap( PIC_SENDQUEUED, OResource::SmallIcon ) )
#define ICON_SHOWFOLDERS    QIconSet( OResource::loadPixmap( PIC_SHOWFOLDERS, OResource::SmallIcon ) )
#define ICON_SEARCHMAILS    QIconSet( OResource::loadPixmap( PIC_SEARCHMAILS, OResource::SmallIcon ) )
#define ICON_EDITSETTINGS   QIconSet( OResource::loadPixmap( PIC_EDITSETTINGS, OResource::SmallIcon ) )
#define ICON_EDITACCOUNTS   QIconSet( OResource::loadPixmap( PIC_EDITACCOUNTS, OResource::SmallIcon ) )
#define ICON_SYNC           QIconSet( OResource::loadPixmap( PIC_SYNC, OResource::SmallIcon ) )
#define ICON_READMAIL       QIconSet( OResource::loadPixmap( PIC_DOCUMENT, OResource::SmallIcon) )

#define PIXMAP_IMAPFOLDER   QPixmap( OResource::loadPixmap( PIC_IMAPFOLDER, OResource::SmallIcon ) )
#define PIXMAP_POP3FOLDER   QPixmap( OResource::loadPixmap( PIC_POP3FOLDER, OResource::SmallIcon ) )
#define PIXMAP_INBOXFOLDER  QPixmap( OResource::loadPixmap( PIC_INBOXFOLDER, OResource::SmallIcon ) )
#define PIXMAP_MBOXFOLDER   QPixmap( OResource::loadPixmap( PIC_MBOXFOLDER, OResource::SmallIcon ) )
#define PIXMAP_OUTBOXFOLDER QPixmap( OResource::loadPixmap( PIC_OUTBOXFOLDER, OResource::SmallIcon ) )
#define PIXMAP_LOCALFOLDER  QPixmap( OResource::loadPixmap( PIC_LOCALFOLDER, OResource::SmallIcon ) )
#define PIXMAP_OFFLINE      QPixmap( OResource::loadPixmap( PIC_OFFLINE, OResource::SmallIcon ) )

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

/* used for making menuids transparent */
#define FOLDER_MENU_REFRESH_HEADER 0
#define FOLDER_MENU_DELETE_ALL_MAILS 1
#define FOLDER_MENU_NEW_SUBFOLDER 2
#define FOLDER_MENU_DELETE_FOLDER 3
#define FOLDER_MENU_MOVE_MAILS 4

#define SERVER_MENU_DISCONNECT 5
#define SERVER_MENU_OFFLINE 6
#define SERVER_MENU_REFRESH_FOLDER 7
#define SERVER_MENU_CREATE_FOLDER 8
#define SERVER_MENU_SUBSCRIBE 9

#endif
