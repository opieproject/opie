// -*- c++ -*-

#ifndef KVNCBOOKMARKDLG_H
#define KVNCBOOKMARKDLG_H

#include <qlist.h>
#include "kvncbookmarkdlgbase.h"
#include "krfbserver.h"


class KVNCBookmarkDlg : public KVNCBookmarkDlgBase
{
		Q_OBJECT

		public:
				KVNCBookmarkDlg ( QWidget * parent=0, const char * name=0, WFlags f=0 ) ;

				~KVNCBookmarkDlg();
				KRFBServer * getServer(QString);
				void addBookmark(KRFBServer*);
				void deleteBookmark(QString);
				QString selectedBookmark(void);

		public slots:
				void readBookmarks();
				void writeBookmarks();

		private:
				void refresh(void);
				QString decipher(const QString& cipher);
				QString encipher(const QString& cipher);
				QList< KRFBServer> servers;
};

#endif // KVNCBOOKMARKDLG_H




