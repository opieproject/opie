#include <qframe.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qfile.h>
#include <qdir.h>
#include <qstring.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qtextstream.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include "krfbserver.h"
#include "kvncbookmarkdlg.h"

KVNCBookmarkDlg::KVNCBookmarkDlg( QWidget * parent, const char * name, WFlags f ) 

: KVNCBookmarkDlgBase( parent, name,f)
{
		readBookmarks();
		refresh();

}

KVNCBookmarkDlg::~KVNCBookmarkDlg()
{
}

void KVNCBookmarkDlg::addBookmark(KRFBServer * server)
{
		if (server) {
				servers.append(server);
				bookmarkList->insertItem(server->name);
				writeBookmarks();
				refresh();
    }
}

void KVNCBookmarkDlg::deleteBookmark(QString name)
{
		KRFBServer * server=0;
    for ( server=servers.first(); server != 0; server=servers.next() ) {
        if (server->name==name) {
						servers.remove(servers.at());
						writeBookmarks();
						refresh();
            return;
				}
    }
}
KRFBServer *KVNCBookmarkDlg::getServer(QString name)
{
		KRFBServer * server=0;
    for ( server=servers.first(); server != 0; server=servers.next() ) {
        if (server->name==name)
          
            return server;
    }
		return 0;
}


/*
  Note that the degree of protection offered by the encryption here is
  only sufficient to avoid the most casual observation of the configuration
  files. People with access to the files can write down the contents and
  decrypt it using this source code.

  Conceivably, and at some burden to the user, this encryption could
  be improved.
*/
QString KVNCBookmarkDlg::encipher(const QString& plain)
{
    // mainly, we make it long
    QString cipher;
    int mix=28730492;
    for (int i=0; i<(int)plain.length(); i++) {
	int u = plain[i].unicode();
	int c = u ^ mix;
	QString x = QString::number(c,36);
	cipher.append(QChar('a'+x.length()));
	cipher.append(x);
	mix *= u;
    }
    return cipher;
}

QString KVNCBookmarkDlg::decipher(const QString& cipher)
{
    QString plain;
    int mix=28730492;
    for (int i=0; i<(int)cipher.length();) {
	int l = cipher[i].unicode()-'a';
	QString x = cipher.mid(i+1,l); i+=l+1;
	int u = x.toInt(0,36) ^ mix;
	plain.append(QChar(u));
	mix *= u;
    }
    return plain;
}

void KVNCBookmarkDlg::readBookmarks(void)
{
		QFile f(QDir::homeDirPath() + QString("/Applications/keypebble/bookmarks"));

		QStringList entry;
		QString key, val;
		KRFBServer * server=0;
		
		if ( f.open(IO_ReadOnly) ) {
				QTextStream t( &f );
				QString s;
				int n = 1;
				while ( !t.eof() ) {
						s = t.readLine();
						

						entry=QStringList::split('=',s);
						key=entry[0].stripWhiteSpace().lower();
						val=entry[1].stripWhiteSpace();

						if (key=="server") {

								if (server){
										servers.append(server);
										server=0;
								}
								server = new KRFBServer();

								if (!server)
										return;
								server->name=val;

						}
						else if (key=="hostname") 
								server->hostname=val;
						else if (key=="password") 
							server->password=decipher(val);
						else if (key=="display")  
								server->display=val.toInt();
						else if (key=="hextile") 
								server->hexTile=val.toInt();
						else if (key=="corre") 
								server->corre=val.toInt();
						else if (key=="rre") 
								server->rre=val.toInt();
						else if (key=="copyrect") 
								server->copyrect=val.toInt();
						else if (key=="colors256") 
								server->colors256=val.toInt();
						else if (key=="shared") 
								server->shared=val.toInt();
						else if (key=="readonly") 
								server->readOnly=val.toInt();
						else if (key=="deiconify") 
								server->deIconify=val.toInt();
						else if (key=="updaterate") 
								server->updateRate=val.toInt();
						else if (key=="scalefactor") 
								server->scaleFactor=val.toInt();

				}
				if (server){
					servers.append(server);
					server=0;
				}
				f.close();
		}
}

void KVNCBookmarkDlg::writeBookmarks(void)
{
		QString filename=Global::applicationFileName("keypebble","bookmarks");
						
		QFile f(filename);

		QString key, val;
		KRFBServer * server=0;
		
		if ( f.open(IO_ReadWrite) ) {
				QTextStream t( &f );
				QString s;
				int n = 1;
        KRFBServer *server;

         for ( server=servers.first(); server != 0; server=servers.next() ) {
		qDebug(server->name);
          t << "server=" << server->name << '\n';
					t << "\thostname=" << server->hostname << '\n';
					t << "\tpassword=" << encipher(server->password )<< '\n';
					t << "\tdisplay=" << server->display << '\n';
					t << "\thextile=" << server->hexTile << '\n';
					t << "\tcorre=" << server->corre << '\n';
					t << "\trre=" << server->rre << '\n';
					t << "\tcopyrect=" << server->copyrect << '\n';
					t << "\tshared=" << server->shared << '\n';
					t << "\treadonly=" << server->readOnly << '\n';
					t << "\tdeiconify=" << server->deIconify << '\n';
					t << "\tupdaterate=" << server->updateRate << '\n';
					t << "\tscalefactor=" << server->scaleFactor << '\n';

				}
				f.close();
		}
}
void KVNCBookmarkDlg::refresh(void)
{
		bookmarkList->clear();
		KRFBServer * server=0;
    for ( server=servers.first(); server != 0; server=servers.next() ) {
      bookmarkList->insertItem(server->name);
    }
}

QString KVNCBookmarkDlg::selectedBookmark()
{
		return bookmarkList->currentText();
}
