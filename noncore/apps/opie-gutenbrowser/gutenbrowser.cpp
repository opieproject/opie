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

#include "editTitle.h"
#include "gutenbrowser.h"
#include "LibraryDialog.h"
#include "optionsDialog.h"
#include "helpme.h"
#include "NetworkDialog.h"

#include "openetext.h"
#include "output.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>

#include <qpe/qpeapplication.h>
#include <qpe/fontdatabase.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/mimetype.h>
#include <qpe/applnk.h>

/* QT */
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

/* STD */
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
    showMainList=TRUE;
    working=false;
    this->setUpdatesEnabled(TRUE);
    QString msg;
    msg="You have now entered unto gutenbrowser,\n";
    msg+="make your self at home, sit back, relax and read something great.\n";

    local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
    setCaption("Gutenbrowser");// Embedded  " VERSION);
    this->setUpdatesEnabled(TRUE);

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
    qDebug("init finished");
    QPEApplication::setStylusOperation( mainList->viewport(),QPEApplication::RightOnHold);

    connect( mainList, SIGNAL( mouseButtonPressed( int, QListBoxItem *,
                                                   const QPoint &)), this,
	     SLOT( mainListPressed(int, QListBoxItem *, const QPoint &)) );

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
        config.setGroup( "Files" );
        QString ramble = config.readEntry( QString::number(i), "" );

        config.setGroup( "Titles" );
        QString tempTitle = config.readEntry( ramble, "");
        config.setGroup( tempTitle);
        int index = config.readNumEntry( "LineNumber", -1 );
        if( index != -1) {
            odebug << tempTitle << oendl;
            if(!tempTitle.isEmpty())
                bookmarksMenu->insertItem( tempTitle);
        }
    }

    QString gutenIndex = local_library + "/GUTINDEX.ALL";
    qDebug("gutenindex "+gutenIndex );
    
    if( QFile( gutenIndex).exists() )
        indexLib.setName( gutenIndex);
    else {
        QString localLibIndexFile = local_library + "/PGWHOLE.TXT";
        newindexLib.setName( localLibIndexFile);
    }
    qDebug("attempting new library");
    LibraryDlg = new LibraryDialog( this, "Library Index" /*, TRUE */);
    loadCheck = false;
    chdir(local_library);
    if(!showMainList) {
        Lview->setFocus();
        for (int i=1;i< qApp->argc();i++) {
            qDebug("Suppose we open somethin");
            if(!load(qApp->argv()[i]))
		return;
        }
    } else {
        fillWithTitles();
        mainList->setFocus();

    }
    writeConfig();
    QTimer::singleShot( 250, this, SLOT(hideView()) );
}

Gutenbrowser::~Gutenbrowser() {
    odebug << "Exit" << oendl;
}


// Google.com search
void Gutenbrowser::InfoBarClick() {
    QString text;
    if( Lview->hasSelectedText()) {
        Lview->copy();
        QClipboard *cb = QApplication::clipboard();
        text = cb->text();
    } else
        text=this->caption();

    searchGoogle(text);
}

// download http with wget or preferred browser
void Gutenbrowser::goGetit( const QString &url, bool showMsg) {
    QString cmd;
    qApp->processEvents();

    QString filename = local_library + "/GUTINDEX.ALL";

    Config cfg("Gutenbrowser");
    cfg.setGroup("FTPsite");
    ftp_host=cfg.readEntry("SiteName", "sailor.gutenberg.org");
    ftp_base_dir= cfg.readEntry("base",  "/pub/gutenberg");
   
    if(!showMsg) {
	//if we just get the gutenindex.all
        cmd="wget -O " + filename +" " + url+" 2>&1" ;
        chdir(local_library);

        Output *outDlg = new Output( 0, tr("Downloading Gutenberg Index...."),
				     TRUE);

        outDlg->showMaximized();
        outDlg->show();
        qApp->processEvents();
        FILE *fp;
        char line[130];
        outDlg->OutputEdit->append( tr("Running wget") );
        outDlg->OutputEdit->setCursorPosition(
                                             outDlg->OutputEdit->numLines() + 1,
                                             0, FALSE
                                             );
        sleep(1);
        fp = popen(  (const char *) cmd, "r");
        if ( fp ) {
            while ( fgets( line, sizeof line, fp)) {
                outDlg->OutputEdit->append(line);
                outDlg->OutputEdit->setCursorPosition(
                                             outDlg->OutputEdit->numLines() + 1,
					     0,FALSE
					             );
            }
            pclose(fp);
            outDlg->OutputEdit->append("Finished downloading\n");
            outDlg->OutputEdit->setCursorPosition(
                                             outDlg->OutputEdit->numLines() + 1,
                                             0, FALSE
                                                 );
            qApp->processEvents();

        }
        outDlg->close();
        delete outDlg;
    } else {
        if( brow == "Konq")
            cmd = "konqueror "+url+" &";
        if( brow == "Opera")
            //for desktop testing
            cmd = "opera "+url+" &";
        if( brow == "Mozilla")
            cmd = "mozilla "+url+" &";
        if( brow == "Netscape")
            cmd = "netscape "+url+" &";
        if(brow == "wget") {
            QString tempHtml=local_library+"webster.html";
            cmd="wget -O "+tempHtml+" -q "+url;
        }

        chdir(local_library);
        system(cmd);
    }
}

void Gutenbrowser::toggleButtonIcons( bool useEm) {
    QString pixDir;
    if(useEm)
        useEm=TRUE;

    pixDir=QPEApplication::qpeDir()+"pics/gutenbrowser";
    odebug << "Docdir is "+QPEApplication::documentDir() << oendl;

    if( useIcons && QDir( pixDir).exists() ) {
        LibraryButton->setPixmap(Opie::Core::OResource::loadPixmap("home", Opie::Core::OResource::SmallIcon ) );
        OpenButton->setPixmap( Opie::Core::OResource::loadPixmap("gutenbrowser/openbook", Opie::Core::OResource::SmallIcon ));
        ForwardButton->setPixmap( Opie::Core::OResource::loadPixmap("forward", Opie::Core::OResource::SmallIcon ));
        BackButton->setPixmap( Opie::Core::OResource::loadPixmap("back", Opie::Core::OResource::SmallIcon ) );
        SearchButton->setPixmap( Opie::Core::OResource::loadPixmap("gutenbrowser/search", Opie::Core::OResource::SmallIcon ) );
        lastBmkButton->setPixmap( Opie::Core::OResource::loadPixmap("gutenbrowser/bookmark_folder", Opie::Core::OResource::SmallIcon ));
        setBookmarkButton->setPixmap( Opie::Core::OResource::loadPixmap("gutenbrowser/bookmark", Opie::Core::OResource::SmallIcon ) );
        dictionaryButton->setPixmap( Opie::Core::OResource::loadPixmap("gutenbrowser/spellcheck", Opie::Core::OResource::SmallIcon ) );
        InfoBar->setPixmap( Opie::Core::OResource::loadPixmap("gutenbrowser/google", Opie::Core::OResource::SmallIcon ));
    }
}


bool Gutenbrowser::queryExit() {
    int exit=QMessageBox::information(this, "Quit...",
                                      "Do your really want to quit?",
                                      QMessageBox::Ok, QMessageBox::Cancel);
    if (exit==1) {
        writeConfig();
        qApp->quit();
    }
    return (exit==1);
}

// SLOT IMPLEMENTATION
void Gutenbrowser::slotFilePrint() {
}

void Gutenbrowser::ByeBye() {
    if (b_queryExit)
        queryExit();
    else
        qApp->quit();
}

void Gutenbrowser::HelpBtn() {
    HelpMe* HelpDlg;
    HelpDlg = new HelpMe( this, "Help Dialog");
    HelpDlg->showMaximized();
}

void Gutenbrowser::DownloadIndex() {
#ifndef Q_WS_QWS
    {
        switch( QMessageBox::information( 0, tr("Download Library Index, "
                                                "or FTP sites?"),
                                          tr("Do you want to download the "
                                             "newest\nProject Gutenberg "
                                             "Library Index?\nor select an ftp "
                                             "site?\n"), tr("&Library Index"),
                                          tr("&Ftp Site"), tr("&Cancel"), 2, 2 )
		                        )
            {
                case 0: // index clicked,
                    downloadLibIndex();
                    break;

                case 1: // ftp selected
                    downloadFtpList();
                    break;
                default:
                    break;
           };
    }
#endif
}


void Gutenbrowser::downloadFtpList() {
    qApp->processEvents();
    optionsDialog* optDlg;
    optDlg = new optionsDialog( this,"Options_Dlg", true);
    optDlg->getSite();
    delete optDlg;
}


void Gutenbrowser::downloadLibIndex() {
    QDir dir( local_library);
    dir.cd("", TRUE);
    goGetit( "http://www.gutenberg.org/dirs/GUTINDEX.ALL", false);
}


void Gutenbrowser::PrintBtn() {
}

void Gutenbrowser::SearchBtn() {
    if( loadCheck) {
        odebug << "loadCheck: we have a loaded doc" << oendl;
        Search();
    }
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
            currentLine++;
        }
        currentFilePos = f.at();
        i_pageNum++;
        pageStopArray.resize(i_pageNum + 1);

        pageStopArray[i_pageNum ] = currentFilePos;
        odebug << currentFilePos << " current page is number " << i_pageNum
               << ", pagesize " << pageSize << ", length " << Lview->length()
               << ", current " << pageStopArray[i_pageNum] << oendl;
        setStatus();

    } else
        odebug << "bal" << oendl;

    Lview->setFocus();

    odebug << "page number " << i_pageNum << " line number " << currentLine << "" << oendl;
}


void Gutenbrowser::BackBtn() {
    if( i_pageNum > 0) {
        int pageSize= Lview->PageSize();

        i_pageNum--;
        currentFilePos = f.at();

        odebug << currentFilePos << " move back to " << pageStopArray[i_pageNum - 1 ]
               << ", current page number " << i_pageNum
               << ", " << pageSize << ", length " << Lview->length() << oendl;

        if(  i_pageNum < 2)
            f.at( 0);
        else {
            if(!f.at( pageStopArray[i_pageNum - 1] ))
                odebug << "File positioned backward did not work" << oendl;
        }
        QString s;
        Lview->clear();

        for(int fd = 0; fd < pageSize ;fd++) {
            f.readLine(s, 256);
            if(useWrap)
                s.replace(QRegExp("\n"),"");
            currentLine++;
            Lview->insertLine( s, -1);
        }

        if( !BackButton->autoRepeat() && !BackButton->isDown()) {
            QString topR;
            QString lastR;
            QString pageR;
            if( i_pageNum < 1)
                i_pageNum = 1;
            setCaption(QString::number(i_pageNum));
        } else {
            Lview->MultiLine_Ex::pageUp( FALSE);
            if( i_pageNum < 1)
                i_pageNum = 1;
            setStatus();
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
            odebug << "top" << oendl;
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

    int pos = 0;//, i = 0;
    int i_topRow = Lview->topRow();
    QString LeftText;// = Lview->text();
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
        }
        if(  LeftText.find( s_pattern, 0 , TRUE) != -1 ||
             LeftText.find( sPattern2, 0 , TRUE) != -1 )
        {
            odebug << "<<<<<< FOUND IT!! new page number " << i_pageNum
                   << ", found at " << currentFilePos << "" << oendl;
            break;
        }
    }
    if(f.atEnd())
        //in case we didnt find anything, we need to show something
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
    qApp->processEvents();
    Lview->deselect();
    Lview->setFocus();
}

// sets the current page and place as a bookmark*/
void Gutenbrowser::setBookmark() {
    int  eexit=QMessageBox::information(this, "Note", "Do you really want to \n"
                                        "set this bookmark?." ,QMessageBox::Yes,
                                        QMessageBox::No);
    if (eexit== 3) {
        currentFilePos = f.at();

        Config cfg("Gutenbrowser");
        cfg.setGroup("General");
        file_name = cfg.readEntry("Current","");
        qApp->processEvents();
        odebug << "Setting book mark "+file_name << oendl;

        cfg.setGroup("Titles");
        title = cfg.readEntry(file_name,"");
        odebug << "title is "+ title << oendl;

        cfg.setGroup( "Bookmarks" );

        cfg.writeEntry("File Name",file_name);
        cfg.writeEntry("Page Number",QString::number(i_pageNum) );
        cfg.writeEntry( "PagePosition",
                        QString::number( pageStopArray[i_pageNum - 1]));
        cfg.writeEntry("Title", title);

        int row =  Lview->topRow();// Lview->Top();
        cfg.writeEntry("LineNumber",QString::number(row) );

        cfg.setGroup(title);
        cfg.writeEntry("File Name",file_name);
        cfg.writeEntry( "LineNumber", QString::number( row));
        cfg.writeEntry( "PagePosition",
                        QString::number( pageStopArray[i_pageNum - 1]));
        cfg.writeEntry( "Page Number", QString::number( i_pageNum) );
        cfg.write();
        bookmarksMenu->insertItem( title);
    }
}


// goes to last set bookmark
void Gutenbrowser::Bookmark( int itemId) {
    Config config("Gutenbrowser");
    config.setGroup( "Bookmarks" );

    odebug << "<<<<<< " << Lview->PageSize() << ", " << Lview->lastRow() - Lview->topRow() << "" << oendl;

    QString itemString;

    odebug << "menu item " << itemId << "" << oendl;
    QString tempTitle;
    QString s_Bmrkrow;
    QString s_pageNum;
    int Bmrkrow=0;
    int bookmarkPosition = 0;

    config.setGroup( "Bookmarks" );
    title = config.readEntry("Title", "");
    file_name = config.readEntry("File Name", "");
    i_pageNum = config.readNumEntry("Page Number", 0);
    bookmarkPosition = config.readNumEntry( "PagePosition",0);
    Bmrkrow = config.readNumEntry("LineNumber",0);
    if( !file_name.contains("/"))
        file_name = local_library + file_name;

    this->setFocus();
    Lview->clear();

    if(!load(file_name))
        return;

    int pageSize = Lview->PageSize();
    f.at(0);
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
    }

    odebug << "bookmark loaded" << oendl;
    setCaption(title);
}


bool Gutenbrowser::load( const char *fileName) {
    odebug << "Title is already set as "+title << oendl;
    odebug << "sizeHint " << sizeHint().height() << " pageSize " << Lview->PageSize() << "" << oendl;
    if( Lview->PageSize() < 4) {
        Lview->setMinimumHeight( sizeHint().height() );
        pointSize = Lview->fontInfo().pointSize();
        odebug << "sizeHint " << sizeHint().height() << " point size " << pointSize << "" << oendl;
        if(pointSize < 15)
            Lview->setFixedVisibleLines(19);
        else
            Lview->setFixedVisibleLines( ( (sizeHint().height() / pointSize ) * 2) -2);
    }

    Config cfg("Gutenbrowser");
    cfg.setGroup("General");
    cfg.writeEntry("Current",fileName);
    cfg.write();
    currentLine=0;

    file_name=fileName;
    QString o_file = fileName;

    i_pageNum = 1;
    odebug << "ready to open "+o_file << oendl;

    if(f.isOpen())
        f.close();

    f.setName( o_file);

    if ( !f.open( IO_ReadOnly)) {
            QMessageBox::message( (tr("Note")), (tr("File not opened sucessfully.\n" +o_file)) );
            return false;
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
        if(useWrap)
            s.replace(QRegExp("\n"),"");
        Lview->insertLine( s,-1);
        currentLine++;
    }

    currentFilePos = f.at();

    pageStopArray[1] = currentFilePos;

    odebug << "<<<<<<<<<<<" << currentFilePos << " current page is number "
           << i_pageNum << ", length " << Lview->length() << ", current "
           << pageStopArray[i_pageNum] << ", pageSize " << Lview->PageSize()
           << oendl;

    Lview->setMaxLines(Lview->PageSize()*2);
    setCaption(title);
    Lview->setAutoUpdate( TRUE);
    loadCheck = true;
    enableButtons(true);

    if( donateMenu->count() == 3)
        donateMenu->insertItem("Current Title", this, SLOT( InfoBarClick() ));

    Lview->setFocus();

    return true;
}

void Gutenbrowser::Search() {
    odebug << "Starting search dialog" << oendl;
    searchDlg = new SearchDialog( this, "Etext Search", true);
    searchDlg->setCaption( tr( "Etext Search" ));
    connect( searchDlg,SIGNAL( search_signal()),this,SLOT( search_slot()));
    connect( searchDlg,SIGNAL( search_done_signal()),this,SLOT( searchdone_slot()));

    QString resultString;
    QString string = searchDlg->searchString;
    Lview->deselect();
    searchDlg->show();
}

void Gutenbrowser::search_slot( ) {
    int line, col;
    if (!searchDlg /*&& !loadCheck */)
        return;

    Lview->getCursorPosition(&line,&col);
    QString to_find_string=searchDlg->get_text();

    if ( last_search != 0 && searchDlg->get_direction() )
        col = col  - pattern.length() - 1 ;

  again:
    int  result = doSearch( to_find_string , TRUE, searchDlg->forward_search(),
                            line, col);
    if(result == 0) {
        if(!searchDlg->get_direction()) {
            // forward search
            int query = QMessageBox::information( searchDlg, "Find",
                                                  "End of document reached.\n"
                                                  "Continue from the "
                                                  "beginning?", "Yes", "No", "",
                                                  0, 1);
            if (query == 0){
								line = 0;
								col = 0;
								goto again;
            }
        } else {
            //backward search
            int query = QMessageBox::information( searchDlg, "Find",
                                                  "End of document reached.\n"
                                                  "Continue from the "
                                                  "beginning?", "Yes", "No", "",
                                                  0, 1);
            if (query == 0){
                QString string = Lview->textLine( Lview->numLines() - 1 );
                line = Lview->numLines() - 1;
                lineCheck = line;
                col  = string.length();
                last_search = -1; //BACKWARD;
                goto again;
            }
        }
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
                    for(int l = 0 ; l < length; l++)
                        Lview->cursorRight(TRUE);

                    int newTop = Lview->Top();
                    if(Lview->lastRow() > i)
                        Lview->ScrollUp( newTop - top);
                } else {
                    for(int l = 0 ; l < length; l++)
                        Lview->cursorRight(TRUE);
                }
                pattern = s_pattern;
                last_search = 1; //FORWARD;
                return 1;
            }
        }
    } else {
        // searching backwards
        QString string;
        for( i = line; i >= 0; i--) {
            string = Lview->textLine(i);
            int line_length = string.length();
            pos = string.findRev(s_pattern, line == i ? col : line_length ,
				 case_sensitive);
            if (pos != -1) {
                length = s_pattern.length();
                if( i <  Lview->Top() ) {
                    Lview->ScrollDown( Lview->PageSize() );
                    Lview->MultiLine_Ex::pageUp( FALSE );
                    if( ! (line == i && pos > col ) ) {
                        for(int l = 0 ; l < length; l++)
                            Lview->cursorRight(TRUE);
                    }
                } else {
                    if( ! (line == i && pos > col ) ) {
                        for( int l = 0 ; l < length; l++)
                            Lview->cursorRight( TRUE);
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
    zipFile="/usr/bin/unzip";
    if( QFile::exists( local_library+newestLibraryFile)) {
        if( QFile::exists(zipFile) )
            UnZipIt(newestLibraryFile);
        else
            QMessageBox::message( "Note",( tr("Please install unzip")) );
    }

    LibraryDlg->setCaption( tr( "Gutenberg Library"));
    Config config("Gutenbrowser");
    config.setGroup("General");

    if(useSplitter)
        LibraryDlg->useSmallInterface=FALSE;

    LibraryDlg->showMaximized();

    if( LibraryDlg->exec() != 0 ) {
        listItemNumber = LibraryDlg->DlglistItemNumber;
        listItemFile =  LibraryDlg->DlglistItemFile;
        listItemYear = LibraryDlg->DlglistItemYear;
        listItemTitle = LibraryDlg->DlglistItemTitle;
        file_name = LibraryDlg->File_Name;
        title = listItemTitle;

        if ( !listItemNumber.isNull()) {
            i_pageNum = 1;
            if( file_name !="Error" && file_name.length() >2 && !file_name.isEmpty() ) {
                if( file_name.find(".zip")) {
                    odebug << "Found zip file\n" << oendl;
                    QString cmd = "/usr/bin/unzip -o " + local_library+file_name +
                                  " -d " + local_library;
                    odebug << "Issuing the command "+ cmd << oendl;
                    system(cmd);
                    remove( file_name);
                }
                delete LibraryDlg;
                setTitle();
                if( !load( file_name)) return;
            } else
                printf("Not opening the file.\n");
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
}

void Gutenbrowser::OpenBtn() {
    QString s_temp;
    s_temp = status;
    OpenEtext* OpenDlg;
    OpenDlg = new  OpenEtext(this,"OpenDlg");
    OpenDlg->showMaximized();

    if( OpenDlg->exec() != 0) {
        title = OpenDlg->openFileTitle;
        odebug << "title open as "+title << oendl;
        file_name = OpenDlg->file;
        i_pageNum = 1;

        if( !file_name.isEmpty() || file_name.length() > 2 ) {
            if(showMainList) {
                showMainList=FALSE;
                odebug << "ShowMainList is now false" << oendl;
                mainList->hide();
                Lview->show();
                qApp->processEvents();
            }
            Lview->clear();
            if(!load(file_name)) return;
        } else {
            odebug << "file_name is empty!" << oendl;
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
    delete OpenDlg;
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

        Lview->setFont(font);

        QRect lRect;
        QRect rRect;
        lRect = Lview->rect();
        update();
    }
#endif
}


// performs dictionary look ups on the web
void Gutenbrowser::LookupBtn() {
    QString text;
    if( Lview->hasSelectedText())
        Lview->copy();

    QClipboard *cb = QApplication::clipboard();
    text = cb->text();
    int  eexit=QMessageBox::information(this, "Note",
                                        "Do you want to lookup\n\"" + text +
                                        "\"\non websters web dictionary?",
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
    } else
        s_file = filer;

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
        }
    }
    return true;
}

void Gutenbrowser::searchdone_slot() {
    this->setFocus();
    last_search = 0;
}

// sets the status message
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
        case Key_D:
            DownloadIndex();
            break;
        case Key_P:
            PrintBtn();
            break;
        case Key_S:
            SearchBtn();
            break;
        case Key_R:
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
        case Key_O:
        case Key_F9: //activity
            OpenBtn();
            break;
        case Key_I:
        case Key_F10: //contacts
            hideButtons();
            break;
        case Key_U:// hide menu
        case Key_F11: //menu
            if(menubar->isHidden() )
                menubar->show();
            else
                menubar->hide();
            break;
        case Key_F12: //home
            BeginBtn();
            break;
        case Key_L:
        case Key_F13: //mail
          LibraryBtn();
          break;
        case Key_Space:
            if(loadCheck)
                ForwardBtn();
            break;
        case Key_Right:
            ForwardButton->setFocus();
            ForwardBtn();
            break;
        case Key_Left:
            BackBtn();
            BackButton->setFocus();
            break;
        case Key_E:
        case Key_Escape:
            ByeBye();
            break;
        case Key_B:
        case Key_PageUp:
            BackBtn();
            break;
        case Key_F:
        case Key_PageDown:
            ForwardBtn();
            break;
        case Key_M:
        case Key_Down:
        case Key_Up:
        case Key_Home:
        defaul:
            break;
    };
}

void Gutenbrowser::keyPressEvent( QKeyEvent *e) {
}

void Gutenbrowser::resizeEvent( QResizeEvent *ev) {
    if( !LibraryDlg->isHidden())
        LibraryDlg->resize(ev->size().width(),ev->size().height() );
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

        odebug << "writing library config" << oendl;
        Config config("Gutenbrowser");
        config.setGroup("General");

        QString dirname= optDlg->downloadDirEdit->text();
        if(dirname.right(1)!="/")
            dirname+="/";

        config.writeEntry( "DownloadDirectory",dirname);
        QDir newDir( optDlg->downloadDirEdit->text());

        if( !newDir.exists() ) {
            int exit=QMessageBox::information(this, "Note",
                                              "Ok, to make a new directory\n" +
                                              Ddir + " ?", QMessageBox::Ok,
                                              QMessageBox::Cancel);
            if (exit==1) {
                QString cmd="mkdir -p ";
                cmd+=Ddir.latin1();
                system(cmd);
                odebug << "Making new dir "+cmd << oendl;
                if(Ddir.right(1)!="/")
                    Ddir+="/";

                config.writeEntry("DownloadDirectory",Ddir);
            }
        }

        if(optDlg->b_qExit==TRUE)
            b_queryExit=TRUE;
        else
            b_queryExit=FALSE;

        if(optDlg->fontDlg-> changedFonts) {
            odebug << "Setting font" << oendl;
            myFont=optDlg->fontDlg->selectedFont;
            Lview->setFont( myFont);
        }

        if(optDlg->useWordWrap_CheckBox->isChecked() ) {
            odebug << "WORD WRAP is set" << oendl;
            Lview->setWordWrap(QMultiLineEdit::WidgetWidth);
            useWrap=true;
        } else {
            odebug << "Word wrap is NOT set" << oendl;
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

        if( strcmp(ramble, file_name) == 0)
            test = 1;
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
    return true;
}

// Calls new fangled network dialog
void Gutenbrowser::OnNetworkDialog( const QString &/*networkUrl*/, const QString &/*output*/)
{
}

void Gutenbrowser::donateGutenberg()
{
    int exit=QMessageBox::information(this, "Info", "http://www.gutenberg.org\n"
                                      "donate@gutenberg.net", QMessageBox::Ok,
                                      QMessageBox::Cancel);
}

void Gutenbrowser::donateByteMonkie()
{
}

void Gutenbrowser::writeConfig()
{
}

void Gutenbrowser::annotations()
{
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
    if(f.isOpen())
        f.close();
}

void Gutenbrowser::fixKeys()
{
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
        if( !temp.isEmpty())
            mainList->insertItem ( Opie::Core::OResource::loadPixmap(
                                                "gutenbrowser/gutenbrowser_sm",
                                                Opie::Core::OResource::SmallIcon
                                                                    ),
                                    temp, -1);
    }
}

void Gutenbrowser::listClickedSlot( QListBoxItem * index) {
    if(index) {
        title = index->text();
        showMainList=FALSE;
        mainList->hide();
        Lview->show();
        qApp->processEvents();
        QString temp;
        temp.setNum( mainList->currentItem() + 1);
        Config config("Gutenbrowser");
        config.setGroup( "Files" );
        QString file = config.readEntry(temp, "");
        odebug << "About to load" << oendl;
        if( Lview->isVisible())
            if(!load(file))
                return;

        config.setGroup( title);
        file_name = config.readEntry("File Name", "");
        i_pageNum = config.readNumEntry("Page Number", 1);
        int Bmrkrow = config.readNumEntry("LineNumber", -1);
        if(Bmrkrow > -1) {
            if( Bmrkrow >  Lview->topRow() )
                Lview->ScrollUp( Bmrkrow  -  Lview->topRow() );
            else if( Bmrkrow <  Lview->topRow() )
                Lview->ScrollDown( Lview->topRow() - Bmrkrow );
        }
    }
}

void Gutenbrowser::infoGutenbrowser() {
    QMessageBox::message("Info", "Gutenbrowser was developed by\nLorn Potter\n"
                         "ljp<ljp@llornkcor.com>\n");
}

void Gutenbrowser::setDocument(const QString & frozenBoogers) {
    qWarning("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXSET DOCUMENT\n");
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
    qWarning("open "+frozenBoogers);
    title=lnk.comment();
    if(!load( lnk.file()))
	return;

    this->show();
}

void Gutenbrowser::mainListPressed(int mouse, QListBoxItem * , const QPoint & )
{
    if (mouse == 2) {
        QPopupMenu  m;
        m.insertItem( tr( "Open" ), this, SLOT( menuOpen() ));
        m.insertSeparator();
        m.insertItem( tr( "Search google.com" ), this, SLOT( menuGoogle()));
        m.insertItem( tr( "Edit Title" ), this, SLOT( menuEditTitle()));
        m.exec( QCursor::pos() );
    }
}

void Gutenbrowser::menuOpen() {
    listClickedSlot( mainList->item( mainList->currentItem()));
}

void Gutenbrowser::menuGoogle() {
    searchGoogle(  mainList->text(mainList->currentItem()));
}

void Gutenbrowser::searchGoogle( const QString &tempText) {
    QString text = tempText;

    int eexit=QMessageBox::information(this, "Note",
                                       "Do you want to search for\n" + text +
                                       "\non google.com?", QMessageBox::Yes,
                                       QMessageBox::No);
    if (eexit== 3) {
        qApp->processEvents();
        text.replace( QRegExp("\\s"), "%20");
        text.replace( QRegExp("#"), "");
        text.replace( QRegExp(","), "%20");
        text.replace( QRegExp("'"), "%20");
        text.replace( QRegExp("("), "");
        text.replace( QRegExp(")"), "");
        QString s_lookup = "http://google.com/search?q="+text+"&num=30&sa=Google+Search";
        goGetit( s_lookup, true);
    }
}

void Gutenbrowser::menuEditTitle()
{
    int currentItem = mainList->currentItem();
    QString title_text = mainList->text( currentItem);

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

        if(title_text == file_title ) {
            i=i_numofFiles+1;
            Edit_Title *titleEdit;
            titleEdit = new Edit_Title(this,file_title ,TRUE);
            if(titleEdit->exec() !=0) {
                config.writeEntry( s_filename, titleEdit->newTitle);
                mainList->removeItem(currentItem);
                mainList->insertItem(Opie::Core::OResource::loadPixmap(
                                                "gutenbrowser/gutenbrowser_sm",
                                                Opie::Core::OResource::SmallIcon
                                                     ),
                                     titleEdit->newTitle, currentItem);
            }
        }
    }

    mainList->triggerUpdate(true);
}



bool Gutenbrowser::UnZipIt(const QString &zipFile) {
    odebug << zipFile << oendl;
#ifndef Q_WS_QWS
    if( QFile::exists( zipFile)) {
        // TODO find some other way of dealing with zip files.
        bool isPgWhole=false;
        QString thatFile;
        if(zipFile.find("pgwhole",0,TRUE)) {
            isPgWhole=TRUE;
            thatFile= local_library +"PGWHOLE.TXT";
        }
        QString cmd;
#if defined(_WS_X11_)
        if( chdir((const char*)local_library.latin1())!=0)
            odebug << "chdir failed." << oendl;

        cmd = "gunzip -S .zip " + local_library+zipFile;
#endif
#ifdef Q_WS_QWS
        if( chdir((const char*)local_library.latin1())!=0)
            odebug << "chdir failed." << oendl;

        cmd = "unzip " + local_library+zipFile;
#endif
        int exit=QMessageBox::information(this, "Ok to unzip?", "Ok to unnzip\n"+ zipFile+" ?", QMessageBox::Yes, QMessageBox::No);
        if (exit==QMessageBox::Yes) {
#if defined(_WS_X11_)
            odebug << "Issuing the command "+cmd << oendl;
            system( cmd);
            if( QFile::exists(thatFile))
                remove (thatFile);
            if(isPgWhole) {
                if( rename("pgwhole","PGWHOLE.TXT") !=0)
                    odebug << "rename failed" << oendl;
            } else {
                if( rename(thatFile.left(4),thatFile.left(4)+".txt") !=0)
                    odebug << "rename failed" << oendl;
            }
#endif
#ifdef Q_WS_QWS
            odebug << "Issuing the command "+cmd << oendl;
            system( cmd);
            if( QFile::exists(thatFile))
                remove(thatFile);
            if(isPgWhole) {
                if( rename("pgwhole","PGWHOLE.TXT") !=0)
                    odebug << "rename failed" << oendl;
            } else {
                if( rename(thatFile.left(4),thatFile.left(4)+".txt") !=0)
                    odebug << "rename failed" << oendl;
            }
#endif
            return true;
        } else if(exit==QMessageBox::No)
            return false;
    } else
        QMessageBox::message( "Note",( tr("Please install unzip in your PATH")) );
#endif
    return false;
}
