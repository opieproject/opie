/***************************************************************************
                      gutenbrowser.cpp  -  description
                      -------------------
 begin                : Mon Jul 24 22:33:12 MDT 2000
 copyright            : (C) 2000 -2004 by llornkcor
 email                : ljp@llornkcor.com
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
// http search
//        http://digital.library.upenn.edu/books/authors.html
//        http://digital.library.upenn.edu/books/titles.html
// ftp://ibiblio.org/pub/docs/books/gutenberg/GUTINDEX.ALL
// donate@gutenberg.net
#include <qpe/qpeapplication.h>
#include <qpe/fontdatabase.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/mimetype.h>
#include <qpe/resource.h>
#include <qpe/applnk.h>

#include "editTitle.h"
#include "gutenbrowser.h"
#include "LibraryDialog.h"
  //#include "bookmarksdlg.h"
#include "optionsDialog.h"
#include "helpme.h"
#include "NetworkDialog.h"

#include "openetext.h"
#include "output.h"


#include <qregexp.h>
#include <qregion.h>
#include <qpaintdevicemetrics.h>
#include <qvaluelist.h>
#include <qlabel.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qobjectlist.h>
#include <qfontdialog.h>
#include <qtextview.h>
#include <qbrush.h>
#include <qfile.h>
#include <qfontinfo.h>
#include <qscrollview.h>
#include <qpoint.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

static const int nfontsizes = 9;
static const int fontsize[nfontsizes] = {8,9,10,11,12,13,14,18,24};

#ifdef NOQUICKLAUNCH
Gutenbrowser::Gutenbrowser()
		Gutenbrowser();
#else
Gutenbrowser::Gutenbrowser(QWidget *,const char*, WFlags )
#endif
				: QMainWindow()
 {
			//  QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
			//     QCopEnvelope e("QPE/System", "grabKeyboard(QString)" );
			//     e << "";
			//    QPEApplication::grabKeyboard();
    showMainList=TRUE;
		working=false;
		this->setUpdatesEnabled(TRUE);
			//  #ifndef Q_WS_QWS
		QString msg;
		msg="You have now entered unto gutenbrowser,\n";
		msg+="make your self at home, sit back, relax and read something great.\n";

		local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
		setCaption("Gutenbrowser");// Embedded  " VERSION);
		this->setUpdatesEnabled(TRUE);

			//    bool firstTime=FALSE;
		topLayout = new QVBoxLayout( this, 0, 0, "topLayout");

		menu = new QHBoxLayout(-1,"menu");
		buttons2 = new QHBoxLayout(-1,"buttons2");
		edits = new QHBoxLayout(-1,"edits");

		useSplitter=TRUE;

		initConfig();
		initMenuBar();
		initButtonBar();
		initStatusBar();
		initView();
		initSlots();

		QPEApplication::setStylusOperation( mainList->viewport(),QPEApplication::RightOnHold);

		connect( mainList, SIGNAL( mouseButtonPressed( int, QListBoxItem *, const QPoint &)),
						 this, SLOT( mainListPressed(int, QListBoxItem *, const QPoint &)) );
		if( useIcons)
				toggleButtonIcons( TRUE);
		else
				toggleButtonIcons( FALSE);

		enableButtons(false);

		Config config("Gutenbrowser"); // populate menubuttonlist
		config.setGroup("General");

		config.setGroup( "Files" );
		QString s_numofFiles = config.readEntry("NumberOfFiles", "0" );
		int  i_numofFiles = s_numofFiles.toInt();

		QString tempFileName;

		for (int i = 0; i <= i_numofFiles; i++) {
					//       tempFileName.setNum(i);
				config.setGroup( "Files" );
				QString ramble = config.readEntry( QString::number(i), "" );

				config.setGroup( "Titles" );
				QString tempTitle = config.readEntry( ramble, "");
				config.setGroup( tempTitle);
				int index=config.readNumEntry( "LineNumber", -1 );
				if( index != -1) {
							//        qDebug( tempTitle);
						bookmarksMenu->insertItem( tempTitle);
				}
		}

			//    QString gutenIndex= local_library + "GUTINDEX.ALL";
		QString gutenIndex= QPEApplication::qpeDir()+ "/etc/gutenbrowser/GUTINDEX.ALL";

		if( QFile( gutenIndex).exists() ) {
				indexLib.setName( gutenIndex);
		} else {
				QString localLibIndexFile = QPEApplication::qpeDir()+ "/etc/gutenbrowser/PGWHOLE.TXT";
					//        QString localLibIndexFile= local_library + "PGWHOLE.TXT";
				newindexLib.setName( localLibIndexFile);
		}
		LibraryDlg = new LibraryDialog( this, "Library Index" /*, TRUE */);
		loadCheck=false;
		chdir(local_library);
		if(!showMainList) {
				Lview->setFocus();
					//          if(firstTime)
					//              Bookmark();
				for (int i=1;i< qApp->argc();i++) {
						qDebug("Suppose we open somethin");
						load(qApp->argv()[i]);
				}
		} else {
				fillWithTitles();
				mainList->setFocus();
					//        mainList->setCurrentItem(0);

		}
		writeConfig();
} //end init

Gutenbrowser::~Gutenbrowser() {
			//  QPEApplication::grabKeyboard();
			//  QPEApplication::ungrabKeyboard();
		qDebug("Exit");
}

  /*
		Google.com search */
void Gutenbrowser::InfoBarClick() {
		QString text;
		if( Lview->hasSelectedText()) {
				Lview->copy();
				QClipboard *cb = QApplication::clipboard();
				text = cb->text();
		} else {
					//    text=title;
				text=this->caption();
		}
		searchGoogle(text);
}

  /*
		download http with wget or preferred browser */
void Gutenbrowser::goGetit( const QString &url, bool showMsg) {
			//    int eexit=0;
		QString cmd;
			//    config.read();
		qApp->processEvents();
		QString filename = QPEApplication::qpeDir();
		if(filename.right(1)!="/")
				filename+="/etc/gutenbrowser/";
		else
				filename+="etc/gutenbrowser/";
		qDebug("filename "+filename);
			//    QString filename = QDir::homeDirPath()+"/Applications/gutenbrowser/";

		filename += url.right( url.length() - url.findRev("/",-1,TRUE) -1);

		Config config("Gutenbrowser");
		config.setGroup( "Browser" );
		QString brow = config.readEntry("Preferred", "Opera");
		qDebug("Preferred browser is "+brow);
		if(!showMsg) { //if we just get the gutenindex.all
				cmd="wget -O " + filename +" " + url+" 2>&1" ;
				chdir(local_library);
				qDebug("Issuing the system command: %s", cmd.latin1());

				Output *outDlg;
				outDlg = new Output(this, tr("Gutenbrowser Output"),FALSE);
				outDlg->showMaximized();
				outDlg->show();
				qApp->processEvents();
				FILE *fp;
				char line[130];
				outDlg->OutputEdit->append( tr("Running wget") );
				sleep(1);
				fp = popen(  (const char *) cmd, "r");
					//                 qDebug("Issuing the command\n"+cmd);
					//                 system(cmd);
				while ( fgets( line, sizeof line, fp)) {
						outDlg->OutputEdit->append(line);
						outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
				}
				pclose(fp);        
				outDlg->close();
				if(outDlg)
						delete outDlg;
		} else {
				if( brow == "Konq") {
						cmd = "konqueror "+url+" &";
				}
				if( brow == "Opera")  { //for desktop testing
						cmd = "opera "+url+" &";
				}
					//              if( brow == "Opera")  { // on Zaurus
					//                  cmd = "operagui "+url+" &";
					//              }
				if( brow == "Mozilla")  {
						cmd = "mozilla "+url+" &";
				}
				if( brow == "Netscape")  {
						cmd = "netscape "+url+" &";
				}
				if(brow == "wget") {
							//                cmd="wget -q "+url+" &";
						QString tempHtml=local_library+"webster.html";
						cmd="wget -O "+tempHtml+" -q "+url;
				}

				chdir(local_library);
				qDebug("Issuing the sys command: %s", cmd.latin1());
				system(cmd);
		}
}

void Gutenbrowser::toggleButtonIcons( bool useEm) {
		QString pixDir;
		if(useEm)
				useEm=TRUE;
		pixDir=QPEApplication::qpeDir()+"pics/gutenbrowser";
		qDebug("Docdir is "+QPEApplication::documentDir());

		if( useIcons && QDir( pixDir).exists() ) {
				LibraryButton->setPixmap( Resource::loadPixmap("home") ); //in inline
				OpenButton->setPixmap( Resource::loadPixmap("gutenbrowser/openbook"));
				ForwardButton->setPixmap( Resource::loadPixmap("forward"));//in inline
				BackButton->setPixmap( Resource::loadPixmap("back") );//in inline
				SearchButton->setPixmap( Resource::loadPixmap("gutenbrowser/search") );//in inline
				lastBmkButton->setPixmap( Resource::loadPixmap("gutenbrowser/bookmark_folder")); 
				setBookmarkButton->setPixmap( Resource::loadPixmap("gutenbrowser/bookmark") );
				dictionaryButton->setPixmap( Resource::loadPixmap("gutenbrowser/spellcheck") );
				InfoBar->setPixmap( Resource::loadPixmap("gutenbrowser/google"));
		}
}


bool Gutenbrowser::queryExit()
{
		int exit=QMessageBox::information(this, "Quit...", "Do your really want to quit?",
																			QMessageBox::Ok, QMessageBox::Cancel);
		if (exit==1) {
				writeConfig();
				qApp->quit();
		} else {
		};
		return (exit==1);
}

  // SLOT IMPLEMENTATION

void Gutenbrowser::slotFilePrint() {
}

void Gutenbrowser::ByeBye() {
		if (b_queryExit)
				queryExit();
		else {
					//        writeConfig();
				qApp->quit();
		}
}

void Gutenbrowser::HelpBtn() {
		HelpMe* HelpDlg;
		HelpDlg = new HelpMe( this, "Help Dialog");
		HelpDlg->showMaximized();
}

void Gutenbrowser::DownloadIndex() {
#ifndef Q_WS_QWS
		{
				switch( QMessageBox::information( 0, (tr("Download Library Index, or FTP sites?")),
																					(tr("Do you want to download the newest\n"
																							"Project Gutenberg Library Index?\n"
																							"or select an ftp site?\n")),
																					(tr("&Library Index")), (tr("&Ftp Site")), (tr("&Cancel")), 2, 2 ) )
          {
						case 0: // index clicked,
								downloadLibIndex();
								break;

						case 1: // ftp selected
								downloadFtpList();
								break;

						case 2: // Cancel
								break;
          };
		}
#endif
} // end DownloadIndex


void Gutenbrowser::downloadFtpList() {
			//  QString cmd="wget http://www.promo.net/pg/list.html");
			//system(cmd);
		qApp->processEvents();
		optionsDialog* optDlg;
		optDlg = new optionsDialog( this,"Options_Dlg", true);
		optDlg->getSite();
		if(optDlg)
				delete optDlg;
}


void Gutenbrowser::downloadLibIndex() {
			// QString dwmloader = local_library + "pgwhole.zip";
			// QString cmd = "lynx -source http://www.gutenberg.net/pgwhole.zip | cat >> " + dwmloader;
			//                  system(cmd);

			//                QString outputFile= local_library+ "GUTINDEX.ALL";
			//                config.setGroup( "FTPsite" );  // ftp server config
			//                ftp_host=config.readEntry("SiteName", "");
			//                ftp_base_dir= config.readEntry("base",  "");
			//                QString networkUrl= "ftp://"+ftp_host+ftp_base_dir+"/GUTINDEX.ALL";
		QDir dir( local_library);
		dir.cd("", TRUE);
		goGetit( "http://sailor.gutenberg.org/GUTINDEX.ALL", false); // until ghttp works on binaries -qt3
			//    goGetit( "http://www.gutenberg.net/pgwhole.zip", true); // until ghttp works on binaries -qt3
			//                NetworkDialog *NetworkDlg;
			//                NetworkDlg = new NetworkDialog( this,"Network Protocol Dialog", TRUE, 0, networkUrl, outputFile );
			//                if( NetworkDlg->exec() != 0 )
			//                { // use new, improved, *INSTANT* network-dialog-file-getterer
			////              QMessageBox::message("Note","");
			//                }
			// if(NetworkDlg)
			//    delete NetworkDlg;
}


void Gutenbrowser::PrintBtn() {
}

void Gutenbrowser::SearchBtn() {
		if( loadCheck) {
				qDebug("loadCheck: we have a loaded doc");
				Search();
		}
			//  else
			//  QMessageBox::message("Note","Sorry, can't search. No etext is loaded");
}


void Gutenbrowser::ForwardBtn() {

		if( !ForwardButton->autoRepeat() && !ForwardButton->isDown()) {

				QString s;
				QString insertString;
				int pageSize= Lview->PageSize();
				Lview->clear();

				for(int fd=0; fd < pageSize - 1;fd++) {
						f.readLine(s, 256);
						if(useWrap)
								s.replace(QRegExp("\n"),"");
						insertString+=s;
						Lview->insertLine( s, -1);
							//            qDebug(s);
						currentLine++;      
				}
					//        Lview->insertAt( insertString,0,0, FALSE);
				currentFilePos = f.at();
					//        if( i_pageNum != pages) {
					//        Lview->MultiLine_Ex::pageDown( FALSE);
				i_pageNum++;
				pageStopArray.resize(i_pageNum + 1);
					//          int length = Lview->length();

				pageStopArray[i_pageNum ] = currentFilePos;
					//         qDebug("%d current page is number %d, pagesize %d, length %d, current %d",
					//                currentFilePos, i_pageNum, pageSize, Lview->length(), pageStopArray[i_pageNum] );
				setStatus();
				Lview->setCursorPosition( 0, 0, FALSE);
					//        }

		} else {
					//         qDebug("bal");
					//         if( i_pageNum != pages) {

					// //          int newTop = Lview->Top();
					//               //      if(Lview->lastRow() > i)
					//             Lview->ScrollUp(1);
					//               //      i_pageNum++;
					//             setStatus();
					//             Lview->setCursorPosition( Lview->Top(), 0, FALSE);

					//         }

		}
		Lview->setFocus();

			//    qDebug("page number %d line number %d", i_pageNum, currentLine);
}


void Gutenbrowser::BackBtn() {
		if( i_pageNum > 0) {
				int pageSize= Lview->PageSize();
					//        int length=Lview->length();

				i_pageNum--;
				currentFilePos = f.at();

					//         qDebug("%d move back to %d, current page number %d, %d, length %d",
					//                currentFilePos, pageStopArray[i_pageNum - 1 ], i_pageNum, pageSize, Lview->length() );

				if(  i_pageNum < 2) {
						f.at( 0);
				} else {
						if(!f.at( pageStopArray[i_pageNum - 1] ))
								qDebug("File positioned backward did not work");
				}
				QString s;
					//        int sizeLine=0;
				Lview->clear();
					//        QString insertString;

				for(int fd = 0; fd < pageSize ;fd++) {
							//        Lview->removeLine( Lview->PageSize() );
						f.readLine(s, 256);
						if(useWrap)
								s.replace(QRegExp("\n"),"");
						currentLine++;
							//            insertString+=s;
						Lview->insertLine( s, -1);
				}

					//        Lview->insertAt( insertString,0,0, FALSE);

				if( !BackButton->autoRepeat() && !BackButton->isDown()) {
						QString topR;
						QString lastR;
						QString pageR;
							//    int sizer =  Lview->lastRow() - Lview->topRow();
							//    int i_topRow = Lview->topRow();
						if( i_pageNum < 1)
								i_pageNum = 1;
            setCaption(QString::number(i_pageNum));
				} else {
							//      int newTop = Lview->Top();
							//      if(Lview->lastRow() > i)
						Lview->MultiLine_Ex::pageUp( FALSE);
							//      Lview->ScrollDown(1);
							//            i_pageNum--;
						if( i_pageNum < 1)
								i_pageNum = 1;
						setStatus();
						Lview->setCursorPosition( Lview->Top(), 0, FALSE);

				}
		}
		Lview->setFocus();
}

void Gutenbrowser::doBeginBtn() {
		if(loadCheck) {
				qApp->processEvents();
				BeginBtn();
		}
}

  // moves text to the very top = 0
void Gutenbrowser::TopBtn() {

		if(loadCheck) {
				if( i_pageNum != 0) {
						qDebug("top");
						qApp->processEvents();
						currentLine = 0;
						i_pageNum = 1;
						int pageSize = Lview->PageSize() ;
						Lview->clear();
						QString s;
						f.at(0);
						for(int fd=0; fd <  pageSize ;fd++) {
								f.readLine(s, 256);
								if(useWrap)
										s.replace(QRegExp("\n"),"");
								Lview->insertLine(s ,-1);
								currentLine++;
						}
						Lview->setCursorPosition( 0,0, FALSE);
						i_pageNum=1;
						setStatus();
				}
				Lview->setFocus();
		}
}

  //moves text to the start of the EText
void Gutenbrowser::BeginBtn() {
		i_pageNum = 1;
		currentLine = 0;
		QString s_pattern="*END*THE SMALL PRINT";
		QString sPattern2 = "*END THE SMALL PRINT";

		int pageSize = Lview->PageSize();
		Lview->clear();

			//    int lines = Lview->numLines();
		int pos = 0;//, i = 0;
		int i_topRow = Lview->topRow();

		QString LeftText;// = Lview->text();

		int linesPerPage = Lview->lastRow() - Lview->topRow();
			//    int pages = (( linesPerPage / Lview->editSize() ) ) +1;
			//    int pageN = 0;
		i_pageNum = 1;
		int lastPage=1;
		int lineNo=0;
		QString s;
		f.at( 0);

		while ( !f.atEnd() ) {
				f.readLine(s, 256);
				lineNo++;
				LeftText = s;
				currentFilePos = f.at();

				i_pageNum = lineNo/pageSize;
				if(lastPage < i_pageNum) {
						pageStopArray.resize(i_pageNum + 1);
						pageStopArray[i_pageNum ] = currentFilePos;
							//             qDebug("new page number %d, found at %d", i_pageNum, currentFilePos);
				}
					//         lastPage =  i_pageNum;  
				if(  LeftText.find( s_pattern, 0 , TRUE) != -1 || LeftText.find( sPattern2, 0 , TRUE) != -1 ) {
						qDebug("<<<<<< FOUND IT!! new page number %d, found at %d", i_pageNum, currentFilePos);
						break;
				}
		}
		if(f.atEnd()) //in case we didnt find anything, we need to show something
				f.at(0);
		Lview->clear();
		for(int fd=0; fd < pageSize - 1;fd++) {
				f.readLine(s, 256);
				if(useWrap)
						s.replace(QRegExp("\n"),"");
				Lview->insertLine( s, -1);
				currentLine++;
		}

		i_pageNum = lineNo/pageSize;
		pageStopArray.resize(i_pageNum + 1);
			//    int length = Lview->length();

		qApp->processEvents();

		if( pos > i_topRow ) {
				Lview->setCursorPosition( pos+linesPerPage+2/* - i_topRow+3 */,0, FALSE);
		} else {
				Lview->setCursorPosition( pos+2 , 0, FALSE );
		}

		Lview->deselect();
			//    AdjustStatus();
		Lview->setFocus();
}

  /*
		sets the current page and place as a bookmark*/
void Gutenbrowser::setBookmark() {
		int  eexit=QMessageBox::information(this, "Note",
																				"Do you really want to \nset this bookmark?."
																				,QMessageBox::Yes, QMessageBox::No);
		if (eexit== 3) {

				currentFilePos = f.at();

				Config cfg("Gutenbrowser");
				cfg.setGroup("General");
				file_name=cfg.readEntry("Current","");
				qApp->processEvents();
				qDebug("Settingbook mark "+file_name);

				cfg.setGroup("Titles");
				title=cfg.readEntry(file_name,"");
				qDebug("title is "+ title);

				cfg.setGroup( "Bookmarks" );

				cfg.writeEntry("File Name",file_name);
				cfg.writeEntry("Page Number",QString::number(i_pageNum) );
				cfg.writeEntry( "PagePosition", QString::number( pageStopArray[i_pageNum - 1]));
				cfg.writeEntry("Title", title);

				int row =  Lview->topRow();// Lview->Top();
				cfg.writeEntry("LineNumber",QString::number(row) );

				cfg.setGroup(title);
				cfg.writeEntry("File Name",file_name);
				cfg.writeEntry( "LineNumber", QString::number( row));
				cfg.writeEntry( "PagePosition", QString::number( pageStopArray[i_pageNum - 1]));
				cfg.writeEntry( "Page Number", QString::number( i_pageNum) );
				bookmarksMenu->insertItem( title);
		}
} //end setBookmark


  /* goes to last set bookmark*/
void Gutenbrowser::Bookmark( int itemId) {

			//    qApp->processEvents();
		Config config("Gutenbrowser");
		config.setGroup( "Bookmarks" );

			//     qDebug("<<<<<< %d, %d", Lview->PageSize(), Lview->lastRow() - Lview->topRow() );

		QString itemString;

		qDebug("menu item %d", itemId);
		QString tempTitle;
		QString s_Bmrkrow;
		QString s_pageNum;
		int Bmrkrow=0;
		int bookmarkPosition = 0;

			//    qApp->processEvents();
		config.setGroup( "Bookmarks" );
		title = config.readEntry("Title", "");
		file_name = config.readEntry("File Name", "");
		i_pageNum = config.readNumEntry("Page Number", 0);
		bookmarkPosition = config.readNumEntry( "PagePosition",0);
		Bmrkrow = config.readNumEntry("LineNumber",0);
		if( !file_name.contains("/")) {
				file_name = local_library + file_name;
		}

			//    getTitle(file_name);
			//    qApp->processEvents();
			//    Lview->setFocus();
		this->setFocus();

		Lview->clear();

		load(file_name);

		int pageSize= Lview->PageSize();
		f.at(0);
			//    Lview->clear();
		QString s;
		int lineNo=0;
		int lastPage=1;
		while ( !f.atEnd() ) {
				f.readLine(s, 256);
				lineNo++;
				currentFilePos = f.at();

				i_pageNum = lineNo/pageSize;
				if(lastPage < i_pageNum) {
						pageStopArray.resize(i_pageNum + 1);
						pageStopArray[i_pageNum ] = currentFilePos;
							//             qDebug("new page number %d, found at %d", i_pageNum, currentFilePos);
				}
				if(currentFilePos == bookmarkPosition)
						break;
		}
		if(f.atEnd())
				f.at(0);
		else
				f.at( bookmarkPosition);

		for(int fd=0; fd < pageSize - 1;fd++) {
				f.readLine(s, 256);
				lineNo++;
				if(useWrap)
						s.replace(QRegExp("\n"),"");
				Lview->insertLine( s, -1);
				currentLine++;
		}

		i_pageNum = lineNo/pageSize;
		pageStopArray.resize(i_pageNum + 1);

		if(showMainList) {
				showMainList=FALSE;
				mainList->hide();
				Lview->show();
					//        qApp->processEvents();
		}

		qDebug("bookmark loaded");
		setCaption(title);
}


bool Gutenbrowser::load( const char *fileName) {

			//    QCopEnvelope ( "QPE/System", "busy()" );
			//    qDebug("Title is already set as "+title);
		qDebug("sizeHint %d pageSize %d", sizeHint().height(),Lview->PageSize() );
		if( Lview->PageSize() < 4) {

				Lview->setMaximumHeight( sizeHint().height() );
				Lview->setMinimumHeight( sizeHint().height() );
				pointSize = Lview->fontInfo().pointSize();
				qDebug("sizeHint %d point size %d", sizeHint().height(), pointSize);
				if(pointSize < 10)
						Lview->setFixedVisibleLines(19);
				else
						Lview->setFixedVisibleLines( ( (sizeHint().height() / pointSize ) * 2) -2);
		}

		Config cfg("Gutenbrowser");
		cfg.setGroup("General");
		cfg.writeEntry("Current",fileName);
		currentLine=0;

		file_name=fileName;
		QString o_file = fileName;

			//  if (i_pageNum < 1) {
		i_pageNum = 1;
			//  }
		qDebug("ready to open "+o_file);

		if(f.isOpen()) f.close();
		f.setName( o_file);

		if ( !f.open( IO_ReadOnly)) {
				QMessageBox::message( (tr("Note")), (tr("File not opened sucessfully.\n" +o_file)) );
				return  false;
		}
		currentFilePos = 0;
		pageStopArray.resize(3);
		pageStopArray[0] = currentFilePos;

		fileHandle = f.handle();
		QString insertString;
		QTextStream t(&f);
		QString s;
		for(int fd=0; fd < Lview->PageSize() ;fd++) {
				s=t.readLine();
					//      insertString+=s;
				if(useWrap)
						s.replace(QRegExp("\n"),"");
					//    s.replace(QRegExp("\r"),"");
				Lview->insertLine( s,-1);
				currentLine++;
		}

			//    int length = Lview->length();
		currentFilePos = f.at();

		pageStopArray[1] = currentFilePos;

		qDebug("<<<<<<<<<<<%d current page is number %d, length %d, current %d, pageSize %d",
					 currentFilePos, i_pageNum, Lview->length(), pageStopArray[i_pageNum], Lview->PageSize() );

		Lview->setMaxLines(Lview->PageSize()*2);
		qDebug("Gulped %d", currentLine);
		setCaption(title);
		Lview->setAutoUpdate( TRUE);

		Lview->setCursorPosition(0,0,FALSE);

			//    pages = (int)(( Lview->numLines() / Lview->editSize() ) / 2 ) +1;
			//qDebug("number of pages %d", pages);

		loadCheck = true;
		enableButtons(true);
		if( donateMenu->count() == 3) {
				donateMenu->insertItem("Current Title", this, SLOT( InfoBarClick() ));
		}
		Lview->setFocus();

			//   QCopEnvelope("QPE/System", "notBusy()" );
		return true;
} // end load

void Gutenbrowser::Search() {

			//    if( searchDlg->isHidden())
		{
				qDebug("Starting search dialog");
				searchDlg = new SearchDialog( this, "Etext Search", true);
				searchDlg->setCaption( tr( "Etext Search" ));
					//        searchDlg->setLabel( "- searches etext");
				connect( searchDlg,SIGNAL( search_signal()),this,SLOT( search_slot()));
				connect( searchDlg,SIGNAL( search_done_signal()),this,SLOT( searchdone_slot()));

				QString resultString;
				QString string = searchDlg->searchString;
				Lview->deselect();
				searchDlg->show();
				searchDlg->result();
		}
}

void Gutenbrowser::search_slot( ) {
    int line, col;
    if (!searchDlg /*&& !loadCheck */)
				return;

    Lview->getCursorPosition(&line,&col);
    QString to_find_string=searchDlg->get_text();

			// searchDlg->get_direction();// is true if searching backward
    if ( last_search != 0 && searchDlg->get_direction() ){
				col = col  - pattern.length() - 1 ;
    }
  again:
    int  result = doSearch( to_find_string , /* searchDlg->case_sensitive()*/ TRUE, searchDlg->forward_search(), line, col);
    if(result == 0){
				if(!searchDlg->get_direction()){ // forward search
						int query = QMessageBox::information( searchDlg, "Find",
																									"End of document reached.\nContinue from the beginning?",
																									"Yes", "No", "", 0,1);
						if (query == 0){
								line = 0;
								col = 0;
								goto again;
						}
				} else { //backward search
						int query = QMessageBox::information( searchDlg, "Find",
																									"End of document reached.\nContinue from the beginning?",
																									"Yes", "No", "", 0,1);
						if (query == 0){
								QString string = Lview->textLine( Lview->numLines() - 1 );
								line = Lview->numLines() - 1;
								lineCheck = line;
								col  = string.length();
								last_search = -1; //BACKWARD;
								goto again;
						}
				}
    } else {

					////    emit CursorPositionChanged(); textLine
    }
}

int Gutenbrowser::doSearch( const QString &s_pattern , bool case_sensitive,  bool forward, int line, int col ) {
		int i, length;
		int pos = -1;
		if(forward) {
				QString string;
				for(i = line; i < Lview->numLines(); i++) {

						string = Lview->textLine(i);
						pos = string.find(s_pattern, i == line ? col : 0, case_sensitive);
						if( pos != -1) {
								int top = Lview->Top();
								length = s_pattern.length();
								if( i > Lview->lastRow() ) {
										Lview->setCursorPosition(i,pos,FALSE);
										for(int l = 0 ; l < length; l++) {
												Lview->cursorRight(TRUE);
										}
										Lview->setCursorPosition( i , pos + length, TRUE );
										int newTop = Lview->Top();
										if(Lview->lastRow() > i)
												Lview->ScrollUp( newTop - top);
											//                    AdjustStatus();
								} else {
										Lview->setCursorPosition(i,pos,FALSE);
										for(int l = 0 ; l < length; l++) {
												Lview->cursorRight(TRUE);
										}
										Lview->setCursorPosition( i , pos + length, TRUE );
											//                    AdjustStatus();
								}
								pattern = s_pattern;
								last_search = 1; //FORWARD;
								return 1;
						}
				}
		} else { //////////////// searching backwards
				QString string;
				for( i = line; i >= 0; i--) {
						string = Lview->textLine(i);
						int line_length = string.length();
						pos = string.findRev(s_pattern, line == i ? col : line_length , case_sensitive);
						if (pos != -1) {
									//                int top = Lview->Top();
								length = s_pattern.length();
								if( i <  Lview->Top() ) {
										Lview->ScrollDown( Lview->PageSize() );
										Lview->MultiLine_Ex::pageUp( FALSE );
										if( ! (line == i && pos > col ) ) {
												Lview->setCursorPosition( i ,pos ,FALSE );
												for(int l = 0 ; l < length; l++) {
														Lview->cursorRight(TRUE);
												}
												Lview->setCursorPosition(i ,pos + length ,TRUE );
													//                        int newTop = Lview->Top();
													/*                        if(useSplitter) Rview->ScrollUp( newTop - top);
													 */                    }
								} else {
										if( ! (line == i && pos > col ) ) {
												Lview->setCursorPosition( i, pos, FALSE );
												for( int l = 0 ; l < length; l++) {
														Lview->cursorRight( TRUE);
												}
												Lview->setCursorPosition( i, pos + length, TRUE );
										}
										pattern = s_pattern;
										last_search = -1;
										return 1;
								}
						}
				}
		}
		return 0;
}

void Gutenbrowser::LibraryBtn() {

		QString newestLibraryFile ="pgwhole.zip";
		QString zipFile;
			//    qDebug("Local Library is %s ", local_library.latin1());
		zipFile="/usr/bin/unzip";
			//    qDebug("newestLibraryFile is %s ", newestLibraryFile.latin1());
		if( QFile::exists( local_library+newestLibraryFile)) {
				if( QFile::exists(zipFile) ) {
						UnZipIt(newestLibraryFile);
				}
				else
						QMessageBox::message( "Note",( tr("Please install unzip")) );
		}
			//    LibraryDlg = new LibraryDialog( this, "Library Index");
		LibraryDlg->setCaption( tr( "Gutenberg Library"));
		Config config("Gutenbrowser");
		config.setGroup("General");
		QString tmp=config.readEntry("RunBefore","FALSE");
		if(tmp=="FALSE") {
				QMessageBox::message( "Note",
															"<P>Your first time running gutenbrowser. You'll need to click the \"load library\" button to load the gutenberg index.</P>");
				config.writeEntry("RunBefore","TRUE");
		}
		if(useSplitter)
				LibraryDlg->useSmallInterface=FALSE;

		LibraryDlg->showMaximized();

		if( LibraryDlg->exec() != 0 ) {
				listItemNumber = LibraryDlg->DlglistItemNumber;
				listItemFile =  LibraryDlg->DlglistItemFile;
				listItemYear = LibraryDlg->DlglistItemYear;
				listItemTitle = LibraryDlg->DlglistItemTitle;
				file_name = LibraryDlg->File_Name;
					//        qDebug("title is being set as "+title);
				title = listItemTitle;

					//                  config.setGroup( "Proxy" );
					//              if( LibraryDlg->checked == 1) {
					//                  config.writeEntry("IsChecked", "TRUE");
					//                  } else {
					//                  config.writeEntry("IsChecked", "FALSE");
					//              }
					//                  config.write();
					//            config.read();
				if ( listItemNumber.isNull()) {
				} else {
						i_pageNum = 1;
						if( file_name !="Error" && file_name.length() >2 && !file_name.isEmpty() ) {
									//replace .zip with txt for opening it.
									//                    printf("\nFile name is now\n");
									//                    printf(file_name);
									//                    printf("\n");
									//
								if( file_name.find(".zip"))
									{
											qDebug("Found zip file\n");
												//              QStringList args;
												//              args="unzip";
												//              args+="-o";
												//              args+=local_library+file_name;
												//              args+="-d";
												//              args+=local_library;
											QString cmd = "/usr/bin/unzip -o " + local_library+file_name + " -d " + local_library;
											qDebug("Issuing the command "+ cmd);
												//  unzipProc=new QProcess( this, "unzipProc" );  /// fark that idea!
												//  unzipProc->start();
											system(cmd);
											remove( file_name);

									}
									//  //
									//                    file_name = file_name.left(4)+ ".txt";
								if( LibraryDlg)
										delete LibraryDlg;
								setTitle();
									//    QCopEnvelope ( "QPE/System", "busy()" );
								load( file_name);
						} else {
								printf("Not opening the file.\n");
						}
				}
		}
		if(showMainList) {
				if(!Lview->isHidden())
						Lview->hide();
				qApp->processEvents();
				showMainList=TRUE;
				if(mainList->isHidden())
						mainList->show();
				fillWithTitles();
				qApp->processEvents();
		} else
				setCentralWidget( Lview);
			//     QPEApplication::grabKeyboard();
			//  fixKeys();
}

void Gutenbrowser::OpenBtn() {
		QString s_temp;
		s_temp = status;
		OpenEtext* OpenDlg;
		OpenDlg = new  OpenEtext(this,"OpenDlg");
		OpenDlg->showMaximized();

		if( OpenDlg->exec() != 0) {
				title = OpenDlg->openFileTitle;
				qDebug("title open as "+title);
				file_name = OpenDlg->file;
				i_pageNum = 1;

				if( !file_name.isEmpty() || file_name.length() > 2 ) {
						if(showMainList) {
								showMainList=FALSE;
								qDebug("ShowMainList is now false");
								mainList->hide();
								Lview->show();
								qApp->processEvents();
						}
						Lview->clear();
							//            QCopEnvelope ( "QPE/System", "busy()" );
						load(file_name);
				} else {
						qDebug("file_name is empty!");
						if(showMainList) {
								if(!Lview->isHidden())
										Lview->hide();
								qApp->processEvents();
								if(mainList->isHidden())
										mainList->show();
								fillWithTitles();
								qApp->processEvents();
						}

				}
		}
		if( OpenDlg)
				delete OpenDlg;
			/*
				Config config("Gutenbrowser");
				config.setGroup( title);
				file_name = config.readEntry("File Name", "");
				i_pageNum = config.readNumEntry("Page Number", -1);
				int Bmrkrow = config.readNumEntry("LineNumber", -1);
				if(Bmrkrow > -1) {
				if( Bmrkrow >  Lview->topRow() ) {
				Lview->setCursorPosition(  Bmrkrow ,0, FALSE );
				Lview->ScrollUp( Bmrkrow  -  Lview->topRow() );
					//            AdjustStatus();
          }
          else if( Bmrkrow <  Lview->topRow() ) {
					Lview->setCursorPosition(  Lview->topRow() - Bmrkrow ,0, FALSE );
					Lview->ScrollDown( Lview->topRow() - Bmrkrow );
						//            AdjustStatus();
						}
						}
			*/
			//    ResizeEdits();

}

void Gutenbrowser::ChangeFont() {
#ifndef Q_WS_QWS
		bool ok;

		weight= Lview->fontInfo().weight();
		italic = Lview->fontInfo().italic();
		bold=Lview->fontInfo().bold();
		pointSize= Lview->fontInfo().pointSize();
		fontStr=Lview->fontInfo().family();

		if(italic == true)
        italicStr="TRUE";
		else
        italicStr="FALSE";

		QFont currentfont( fontStr, pointSize, weight, italic );

		if (ok) {
				QFontInfo fontInfo(font );
				fontStr=fontInfo.family();

				pointSize= fontInfo.pointSize();

				font.setFontSize(pointSize);

				pointSizeStr.setNum( pointSize);

				weight= fontInfo.weight();
				weightStr.setNum( weight);
				italic =fontInfo.italic();
				bold=fontInfo.bold();

				if(italic == true)
						italicStr="TRUE";
				else
						italicStr="FALSE";
				if(bold == true)
						boldStr="TRUE";
				else
						boldStr="FALSE";

        pointSizeStr.setNum( pointSize);
        config.setGroup( "Font" );
        config.writeEntry("Family", fontStr );
        config.writeEntry("Size", pointSizeStr );
        config.writeEntry("Weight", weightStr );
        config.writeEntry("Italic", italicStr );
        config.writeEntry("Bold", boldStr );
					//      config.write();

        Lview->setFont(font);

        QRect lRect;
        QRect rRect;
        lRect = Lview->rect();
        if(useSplitter) {
        }
					//       if(loadCheck) {
					//         ResizeEdits();
					//       }
        update();
		}
#endif
}


  /*
		performs dictionary look ups on the web */
void Gutenbrowser::LookupBtn() {
		QString text;
		if( Lview->hasSelectedText()) {
				Lview->copy();
		}
		QClipboard *cb = QApplication::clipboard();
		text = cb->text();
		int  eexit=QMessageBox::information(this,
																				"Note","Do you want to lookup\n\""+text+"\"\non websters web dictionary?",
																				QMessageBox::Yes, QMessageBox::No);
		if (eexit== 3) {
					// this link for sale!!
				qApp->processEvents();
				goGetit( "http://www.m-w.com/cgi-bin/dictionary?" + text, true);
		}
}

void Gutenbrowser::ClearEdit() {
		Lview->setText("");
		loadCheck = false;
		status = ( tr("Gutenbrowser"));
		InfoBar->setText( "");
		setCaption( tr("Gutenbrowser"));
		i_pageNum = 0;
		enableButtons(false);

		if(!showMainList) {
				Lview->hide();
				showMainList=TRUE;
				mainList->show();
				fillWithTitles();
				qApp->processEvents();
		}
		if(donateMenu->idAt(3) != -1)
				donateMenu->removeItemAt(3);
}


bool Gutenbrowser::getTitle( const char *file ) {
		QString s_file;
		QString filer = file;
		if( filer.contains(local_library, TRUE))  {
				QFileInfo f(file);
				s_file =  f.fileName();
		} else {
				s_file = filer;
		}
		Config config("Gutenbrowser");
		config.setGroup( "Files" );
		QString s_numofFiles = config.readEntry("NumberOfFiles", "0" );
		int  i_numofFiles = s_numofFiles.toInt();

		for (int i = 1; i <= i_numofFiles; i++) {
				QString temp;
				temp.setNum(i);
				QString ramble = config.readEntry(temp, "" );

				if( strcmp(ramble, s_file) == 0) {
						config.setGroup( "Titles" );
						title = config.readEntry(ramble, "");
							//            qDebug("(getTitle)title is being set as "+title);
				}
		}
		return true;
}

void Gutenbrowser::searchdone_slot() {
			// if (!searchDlg)
			//   return;
			// searchDlg->hide();
			//    Lview->setFocus();
		this->setFocus();
    last_search = 0;
			//  ResizeEdits();
}

  /*
		sets the status message */
bool Gutenbrowser::setStatus() {

#ifndef Q_WS_QWS
		QString s_pages;
		s_pages.setNum( pages);
		QString chNum;
		statusTop = status.left( status.find("  ", TRUE) );
		status.append(chNum.setNum( i_pageNum));
		status += " / " + s_pages;

		if(loadCheck) {
				statusBar->message( status);
				InfoBar->setText( title);
		} else {
		}
#else
		QString msg;
		msg.sprintf(title+"    %d", i_pageNum);
		setCaption( msg);
#endif
		return true;
}

void Gutenbrowser::keyReleaseEvent( QKeyEvent *e) {
		switch ( e->key() ) {

			case Key_M:
						//            Bookmark();
					break;
			case Key_D:
					DownloadIndex();
					break;
			case Key_L:
					LibraryBtn();
					break;
			case Key_O:
					OpenBtn();
					break;
			case Key_F:
					ForwardBtn();
					break;
			case Key_B:
					BackBtn();
					break;
			case Key_P:
					PrintBtn();
					break;
			case Key_S:
					SearchBtn();
					break;
			case Key_E:
					ByeBye();
					break;
			case Key_R:
						//            setBookmark();
					break;
			case Key_T:
					ChangeFont();
					break;
			case Key_C:
					ClearEdit();
					break;
			case Key_H:
					HelpBtn();
					break;
			case Key_K:
					LookupBtn();
					break;
			case Key_U:// hide menu
					if(menubar->isHidden() )
              menubar->show();
					else
              menubar->hide();
					break;
			case Key_I:
					hideButtons();
					break;
						////////////////////////////// Zaurus keys
			case Key_Home:
						//          BeginBtn();
					break;
      case Key_F9: //activity
					OpenBtn();
					break;
      case Key_F10: //contacts
					hideButtons();
					break;
      case Key_F11: //menu
					if(menubar->isHidden() )
							menubar->show();
					else
							menubar->hide();
					break;
      case Key_F12: //home
					BeginBtn();
					break;
      case Key_F13: //mail
          LibraryBtn();
          break;
			case Key_Space:
					if(loadCheck)
              ForwardBtn();
						//              else
						//                  Bookmark();
					break;
			case Key_Down:
					if(loadCheck) {
								//              if( !e->isAutoRepeat() )
								//              AdjustStatus();
								//              } else {
								//              LibraryBtn();
								//              ForwardBtn();
					}

						//            ForwardButton->setFocus();
						//              Lview->setFocus();
						//              if(s_Wrap=="FALSE")
						//                Lview->MultiLine_Ex::ScrollDown( 1);
						//            LibraryBtn();
					break;
			case Key_Up:
					if(loadCheck) {
								//              if( !e->isAutoRepeat() )
								//                AdjustStatus();
								//              } else {
								//                  OpenBtn();
								//                  BackBtn();
					}
						//            BackButton->setFocus();
						//              Lview->setFocus();
						//              if(s_Wrap=="FALSE")
						//                  Lview->MultiLine_Ex::ScrollUp( 1);

						//            LibraryBtn();
					break;
			case Key_Right:
					ForwardButton->setFocus();
					ForwardBtn();
						//            LibraryBtn();
					break;
			case Key_Left:
					BackBtn();
					BackButton->setFocus();
						//            OpenBtn();
					break;
			case Key_Escape:
					ByeBye();
					break;
			case Key_PageUp:
					BackBtn();
					break;
			case Key_PageDown:
					ForwardBtn();
					break;

						////////////////////////////// Zaurus keys
		};
}

void Gutenbrowser::keyPressEvent( QKeyEvent *e) {
		switch ( e->key() ) {
					//          case Key_Next:
					//              ForwardBtn();
					//              break;
					//          case Key_Prior:
					//              BackBtn();
					//              break;
					//          case Key_Space:
					//              ForwardBtn();
					//              break;
					//          case Key_Down:
					//            Lview->MultiLine_Ex::ScrollUp( 1);
					//                if(useSplitter) Rview->MultiLine_Ex::ScrollUp( 1);
					//              break;
					//          case Key_Up:
					//            if( Lview->Top() != 0) {
					//              Lview->MultiLine_Ex::ScrollDown( 1);
					//                  if(useSplitter) Rview->MultiLine_Ex::ScrollDown( 1);
					//            }
					//            break;
		}
}

void Gutenbrowser::resizeEvent( QResizeEvent *ev) {
			//       qDebug("resize: %d,%d\n",ev->size().width(),ev->size().height());

		if( !LibraryDlg->isHidden())
				LibraryDlg->resize(ev->size().width(),ev->size().height() );

			//    if(  loadCheck == true) {
			//        ResizeEdits();
			//        AdjustStatus();
			//    }
}


void Gutenbrowser::doOptions() {
		optionsDialog* optDlg;
		optDlg = new optionsDialog( this,"Options_Dlg", true);
		QString Ddir;
		Config config("Gutenbrowser");
		config.setGroup( "General" );
		QFont myFont;
		optDlg->showMaximized();

		if( optDlg->exec() !=0) {
				qApp->processEvents();
				brow=optDlg->browserName;
				toggleButtonIcons( optDlg->useIcon);
				ftp_host= optDlg->ftp_host;
				ftp_base_dir= optDlg->ftp_base_dir;
				brow=optDlg->browserName;
				Ddir=optDlg->downloadDirEdit->text();

				qDebug("writing library config");
				Config config("Gutenbrowser");
				config.setGroup("General");

				QString dirname= optDlg->downloadDirEdit->text();
				if(dirname.right(1)!="/")
						dirname+="/";
				config.writeEntry( "DownloadDirectory",dirname);
				QDir newDir( optDlg->downloadDirEdit->text());

				if( !newDir.exists() ) {
						int exit=QMessageBox::information(this, "Note", "Ok, to make a new directory\n"+Ddir+" ?",
																							QMessageBox::Ok, QMessageBox::Cancel);
						if (exit==1) {
								QString cmd="mkdir -p ";
								cmd+=Ddir.latin1();
								system(cmd);
								qDebug("Making new dir "+cmd);
								if(Ddir.right(1)!="/") {
										Ddir+="/";
								}
								config.writeEntry("DownloadDirectory",Ddir);
						}
				}

//            if(optDlg->styleChanged)
//                setStyle( optDlg->styleInt);
				if(optDlg->b_qExit==TRUE)
						b_queryExit=TRUE;
				else
						b_queryExit=FALSE;

				if(optDlg->fontDlg-> changedFonts) {
						qDebug("Setting font");
						myFont=optDlg->fontDlg->selectedFont;
						Lview->setFont( myFont);
				}

				if(optDlg->useWordWrap_CheckBox->isChecked() ) {
						qDebug("WORD WRAP is set");
						Lview->setWordWrap(QMultiLineEdit::WidgetWidth);
						useWrap=true;
				} else {
						qDebug("Word wrap is NOT set");
						Lview->setWordWrap(QMultiLineEdit::NoWrap);
						useWrap=false;
				}
		}
		if(showMainList) {
				if(!Lview->isHidden())
						Lview->hide();
				qApp->processEvents();
				if(mainList->isHidden())
						mainList->show();
				fillWithTitles();
		} else {
				Lview->show();
				showMainList=FALSE;
				mainList->hide();
		}
		qApp->processEvents();
		update();
}

bool Gutenbrowser::setTitle() {
		if( file_name.contains( local_library)) {
				QFileInfo f( file_name);
				QString s_file =  f.fileName();
				file_name = s_file;
		}

		int test = 0;
		Config config("Gutenbrowser");
		config.setGroup( "Files" );
		QString s_numofFiles = config.readEntry("NumberOfFiles", "0" );
		int  i_numofFiles = s_numofFiles.toInt();

		for (int i = 1; i <= i_numofFiles; i++) {
				QString temp;
				temp.setNum(i);
				QString ramble = config.readEntry(temp, "" );

				if( strcmp(ramble, file_name) == 0) {
						test = 1;
				}
		}
		if (test == 0) {
				config.writeEntry("NumberOfFiles",i_numofFiles +1 );
				QString interger;
				interger.setNum( i_numofFiles +1);
				config.writeEntry(interger, file_name);

				config.setGroup( "Titles" );
				config.writeEntry(file_name,listItemTitle);
		}
		test = 0;
			//     config.write();
		return true;
}
  /*Calls new fangled network dialog */
void Gutenbrowser::OnNetworkDialog( const QString &/*networkUrl*/, const QString &/*output*/)
{
			//     qDebug(networkUrl);
			//     qDebug(output);
			// #ifndef Q_WS_QWS
			//     NetworkDialog *NetworkDlg;
			//     if( networkUrl.length() < 4 ) networkUrl= "http://sailor.gutenberg.org/mirror.sites.html";
			//     NetworkDlg = new NetworkDialog( this,"Network Protocol Dialog", TRUE, 0, networkUrl, output);
			//     if( NetworkDlg->exec() != 0 ) {
			//     }
			//     if(NetworkDlg)
			//         delete NetworkDlg;
			// #endif
}

void Gutenbrowser::donateGutenberg()
{
		int exit=QMessageBox::information(this,	"Info", "http://www.gutenberg.org\ndonate@gutenberg.net",
																			QMessageBox::Ok, QMessageBox::Cancel);

		if (exit==1) {
		} else {
		};
}

void Gutenbrowser::donateByteMonkie()
{
}

void Gutenbrowser::writeConfig()
{
			//   config.read();
			//     Config config("Gutenbrowser");
			// #ifndef Q_WS_QWS
			//    config.setGroup( "Geometry" );
			//     QString s;
			//     QString globalPosition;
			//     QPoint point;
			//     point=mapToGlobal(QPoint(0,0) );
			//     int posiX, posiY;
			//     posiX=point.x();
			//     posiY=point.y();
			//     globalPosition.sprintf("%i,%i", posiX, posiY);

			//     QRect frameRect;
			//     frameRect = geometry();
			//     s.sprintf("%i,%i", frameRect.width(), frameRect.height());
			//     config.writeEntry("Frame", s);
			//     config.writeEntry("Position", globalPosition);

			//     if( useIcons==TRUE)
			//         config.writeEntry("Icons", "TRUE");
			//     else
			//         config.writeEntry("Icons", "FALSE");
			// #endif

			// #ifndef Q_WS_QWS
			//     QString temp;
			//     temp.setNum(styleI, 10);
			//     config.setGroup( "StyleIze" );
			//     config.writeEntry( "Style", temp);
			// #endif

			//     config.setGroup("General");
}

void Gutenbrowser::annotations()
{
			// #ifndef Q_WS_QWS
			// QMessageBox::message("Note","hi annotate");
			// lastBmkButton->setDown(FALSE);
			// #endif
}

void Gutenbrowser::hideButtons()
{

    if( !buttonsHidden) {
				buttonsHidden=TRUE;
				LibraryButton->hide();
				SearchButton->hide();
				InfoBar->hide();
				OpenButton->hide();
				BackButton->hide();
				ForwardButton->hide();
				setBookmarkButton->hide();
				lastBmkButton->hide();
				dictionaryButton->hide();
				if(useSplitter) {
						ClearButton->hide();
						PrintButton->hide();
						beginButton->hide();
						HelpButton->hide();
						FontButton->hide();
						ExitButton->hide();
						OptionsButton->hide();
						DownloadButton->hide();
				}
    } else {
				buttonsHidden=FALSE;

				LibraryButton->show();
				SearchButton->show();
				InfoBar->show();
				OpenButton->show();
				BackButton->show();
				ForwardButton->show();
				setBookmarkButton->show();
				lastBmkButton->show();
				dictionaryButton->show();
				if(useSplitter) {
						ClearButton->show();
						PrintButton->show();
						beginButton->show();
						HelpButton->show();
						FontButton->show();
						ExitButton->show();
						OptionsButton->show();
						DownloadButton->show();
				}
    }
}

void Gutenbrowser::focusInEvent( QFocusEvent* )
{
}
void Gutenbrowser::focusOutEvent( QFocusEvent* )
{
}

void Gutenbrowser::cleanUp(  )
{
		writeConfig();
		QPEApplication::grabKeyboard();

		QPEApplication::ungrabKeyboard();
			//    qDebug("Cleanup");
		if(f.isOpen()) f.close();
}

void Gutenbrowser::fixKeys()
{
		qDebug("Attempting to fix keys");

}

void Gutenbrowser::enableButtons(bool b)
{
		BackButton->setEnabled(b);
		ForwardButton->setEnabled(b);
		SearchButton->setEnabled(b);
		setBookmarkButton->setEnabled(b);
		dictionaryButton->setEnabled(b);
		InfoBar->setEnabled(b);

		editMenu->setItemEnabled( -5, b);
		editMenu->setItemEnabled( -6, b);
		editMenu->setItemEnabled( -7, b);
		editMenu->setItemEnabled( -8, b);

}

void Gutenbrowser::fillWithTitles() {
		Config config("Gutenbrowser");
		config.setGroup( "Files" );
		int  i_numofFiles = config.readNumEntry("NumberOfFiles", 0);
		mainList->clear();
		QString temp;
		for (int i = 1; i <= i_numofFiles; i++) {
				temp.setNum(i);
				config.setGroup( "Files" );
				QString ramble = config.readEntry(temp, "" );
				config.setGroup( "Titles" );
				temp = config.readEntry(ramble, "");
				if( !temp.isEmpty()) {
						mainList->insertItem (  Resource::loadPixmap("gutenbrowser/gutenbrowser_sm"), temp, -1);
				}
		}
}

void Gutenbrowser::listClickedSlot( QListBoxItem * index) {
			//      if( !working) {
			//          working=true;
		if(index) {
					//        QCopEnvelope ( "QPE/System", "busy()" );
				title = index->text();
				showMainList=FALSE;
				mainList->hide();
				Lview->show();
				qApp->processEvents();
				QString temp;
				temp.setNum( mainList->currentItem() + 1);
					//    openFileTitle = title;
				Config config("Gutenbrowser");
				config.setGroup( "Files" );
				QString file = config.readEntry(temp, "");
				qDebug("About to load");
				if( Lview->isVisible())
						load(file);

				config.setGroup( title);
				file_name = config.readEntry("File Name", "");
				i_pageNum = config.readNumEntry("Page Number", 1);
				int Bmrkrow = config.readNumEntry("LineNumber", -1);
				if(Bmrkrow > -1) {
						if( Bmrkrow >  Lview->topRow() ) {
								Lview->setCursorPosition(  Bmrkrow /* - Lview->topRow() */,0, FALSE );
								Lview->ScrollUp( Bmrkrow  -  Lview->topRow() );
									//                AdjustStatus();
						}
						else if( Bmrkrow <  Lview->topRow() ) {
								Lview->setCursorPosition(  Lview->topRow() - Bmrkrow ,0, FALSE );
								Lview->ScrollDown( Lview->topRow() - Bmrkrow );
									//                AdjustStatus();
						}
				}
		}
}

void Gutenbrowser::infoGutenbrowser() {
		QMessageBox::message("Info","Gutenbrowser was developed by\n"
												 "Lorn Potter\n"
												 "ljp<ljp@llornkcor.com>\n");
}

void Gutenbrowser::setDocument(const QString & frozenBoogers) {
		this->hide();
		if(showMainList) {
				showMainList=FALSE;
				if(!mainList->isHidden())
						mainList->hide();
				if(Lview->isHidden())
						Lview->show();
				qApp->processEvents();
		}
		DocLnk lnk;
		lnk = DocLnk(frozenBoogers);
			//    qDebug(lnk.file() );
		title=lnk.comment();
		load( lnk.file());
		this->show();
			//    qDebug(title);
			//      qApp->processEvents();
			//      repaint();
}

void Gutenbrowser::mainListPressed(int mouse, QListBoxItem * , const QPoint & )
{
		switch (mouse) {
			case 1:
					break;
			case 2: {
					QPopupMenu  m;// = new QPopupMenu( Local_View );
					m.insertItem( tr( "Open" ), this, SLOT( menuOpen() ));
					m.insertSeparator();
					m.insertItem( tr( "Search google.com" ), this, SLOT( menuGoogle()));
					m.insertItem( tr( "Edit Title" ), this, SLOT( menuEditTitle()));
						//      m.insertSeparator();
						//      m.insertItem( tr( "Delete" ), this, SLOT( remoteDelete() ));
					m.exec( QCursor::pos() );
			}
					break;
		};
}

void Gutenbrowser::menuOpen() {
		listClickedSlot( mainList->item( mainList->currentItem()));
}

void Gutenbrowser::menuGoogle() {
		searchGoogle(  mainList->text(mainList->currentItem()));
}

void Gutenbrowser::searchGoogle( const QString &tempText) {
		QString text = tempText;

		int  eexit=QMessageBox::information(this, "Note","Do you want to search for\n"+text+
																				"\non google.com?",QMessageBox::Yes, QMessageBox::No);
		if (eexit== 3) {
				qApp->processEvents();
				text.replace( QRegExp("\\s"), "%20");
				text.replace( QRegExp("#"), "");
				text.replace( QRegExp(","), "%20");
				text.replace( QRegExp("'"), "%20");
				text.replace( QRegExp("("), "");
				text.replace( QRegExp(")"), "");
					//        text.replace( QRegExp("[0-9]"), "");
				QString s_lookup = "http://google.com/search?q="+text+"&num=30&sa=Google+Search";
				goGetit( s_lookup, true);
		}

}

void Gutenbrowser::menuEditTitle()
{
		int currentItem = mainList->currentItem();
		QString title_text = mainList->text( currentItem);
			//qDebug("Selected "+title_text);

		Config config("Gutenbrowser");
		config.setGroup( "Files" );
		QString s_numofFiles = config.readEntry("NumberOfFiles", "0");
		int  i_numofFiles = s_numofFiles.toInt();
		QString fileNum;
		for (int i = 1; i <= i_numofFiles+1; i++) {

				fileNum.setNum(i);
				config.setGroup( "Files" );
				QString s_filename = config.readEntry(fileNum, "" );
				config.setGroup( "Titles" );
				QString file_title = config.readEntry( s_filename, "");
					//qDebug("file_title is "+file_title);
				if(title_text == file_title ) {
							//            selFile = s_filename;
							//qDebug("Edit: "+ file_title );
						i=i_numofFiles+1;
						Edit_Title *titleEdit;
						titleEdit = new Edit_Title(this,file_title ,TRUE);
						if(titleEdit->exec() !=0) {
									//qDebug(titleEdit->newTitle);
								config.writeEntry( s_filename, titleEdit->newTitle);
								mainList->removeItem(currentItem);
								mainList->insertItem (  Resource::loadPixmap("gutenbrowser/gutenbrowser_sm"), titleEdit->newTitle, currentItem);
						}
				}
		}

			//    getTitles();
		mainList->triggerUpdate(true);

}



bool Gutenbrowser::UnZipIt(const QString &zipFile) {
		qDebug(zipFile);
#ifndef Q_WS_QWS
		if( QFile::exists( zipFile)) {      // TODO findsome other way of dealingwithzip files.
				bool isPgWhole=false;
				QString thatFile;
				if(zipFile.find("pgwhole",0,TRUE)) {
						isPgWhole=TRUE;
						thatFile= local_library +"PGWHOLE.TXT";
				}
					//          else {
					//              thatFile=zipFile.left(4);
					//          }
					//      qDebug((const char*)local_library.latin1());
				QString cmd;
#if defined(_WS_X11_)
				if( chdir((const char*)local_library.latin1())!=0)
						qDebug("chdir failed.");
        cmd = "gunzip -S .zip " + local_library+zipFile;
					//cmd = "gunzip -d " + zipFile /*newestLibraryFile  */+" -d " + local_library;
#endif
#ifdef Q_WS_QWS
				if( chdir((const char*)local_library.latin1())!=0)
						qDebug("chdir failed.");
        cmd = "unzip " + local_library+zipFile;
					//      cmd = "/usr/bin/unzip -o " + local_library+zipFile +" -d /usr/share/doc/gutenbrowser" ;//+ local_library;
#endif
				int exit=QMessageBox::information(this, "Ok to unzip?",
																					"Ok to unnzip\n"+ zipFile+" ?",
																					QMessageBox::Yes, QMessageBox::No);
				if (exit==QMessageBox::Yes) {
#if defined(_WS_X11_)//
						qDebug("Issuing the command "+cmd);
						system( cmd);
						if( QFile::exists(thatFile))
								remove (thatFile);
						if(isPgWhole) {
								if( rename("pgwhole","PGWHOLE.TXT") !=0)
										qDebug("rename failed");
						} else {
								if( rename(thatFile.left(4),thatFile.left(4)+".txt") !=0)
										qDebug("rename failed");
						}
#endif
#ifdef Q_WS_QWS
						qDebug("Issuing the command "+cmd);
						system( cmd);
						if( QFile::exists(thatFile))
								remove(thatFile);
						if(isPgWhole) {
								if( rename("pgwhole","PGWHOLE.TXT") !=0)
										qDebug("rename failed");
						} else {
								if( rename(thatFile.left(4),thatFile.left(4)+".txt") !=0)
										qDebug("rename failed");
						}
#endif
							//       remove( zipFile);
						return true;
				}
				else if(exit==QMessageBox::No) {
							//            qDebug("unzip");
						return false;
				}
		}
		else
				QMessageBox::message( "Note",( tr("Please install unzip in your PATH")) );
#endif
		return false;
}


