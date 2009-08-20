/***************************************************************************
//                            LibraryDialog.cpp  -  description
//                               -------------------
//      begin                : Sat Aug 19 2000
//      copyright            : (C) 2000 - 2004 by llornkcor
//      email                : ljp@llornkcor.com
//                            ***************************************************/
//  /***************************************************************************
//   *   This program is free software; you can redistribute it and/or modify  *
//   *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
//   *   (at your option) any later version.                                   *
//   ***************************************************************************/
//ftp://ibiblio.org/pub/docs/books/gutenberg/GUTINDEX.ALL

#include "LibraryDialog.h"
#include "output.h"

/* OPIE */
#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpedialog.h>
#include <opie2/odebug.h>

/* QT */
#include <qpushbutton.h>
#include <qmultilineedit.h>

/* STD */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
LibraryDialog::LibraryDialog( QWidget* parent,  const char* name , bool /*modal*/, WFlags fl )
   : QDialog( parent, name, true/* modal*/, fl )
{
   if ( !name )
      setName( "LibraryDialog" );
   indexLoaded=false;
   initDialog();

   index = "GUTINDEX.ALL";
   local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
   local_index = local_library + index;

   QString iniFile ;
   iniFile = local_library + "/gutenbrowserrc";
   new_index = local_library + "/PGWHOLE.TXT";
   old_index = local_index;

   Config config("Gutenbrowser");

   config.setGroup( "HttpServer" );
   proxy_http = config.readEntry("Preferred", "http://sailor.gutenbook.org");

   config.setGroup( "FTPsite" );
   ftp_host = config.readEntry("SiteName", "sailor.gutenberg.org");
   ftp_base_dir= config.readEntry("base",  "/pub/gutenberg");

   i_binary = 0;

   config.setGroup("SortAuth");
   if( config.readEntry("authSort", "false") == "true")
      authBox->setChecked(true);

   config.setGroup("General");
   downDir = config.readEntry( "DownloadDirectory",local_library);
   newindexLib.setName( old_index);
   indexLib.setName( old_index);

   new QPEDialogListener(this);
   QTimer::singleShot( 1000, this, SLOT( FindLibrary()) );

}

void  LibraryDialog::clearItems() {
		ListView1->clear();
		ListView2->clear();
		ListView3->clear();
		ListView4->clear();
		ListView5->clear();
}

/*This groks using PGWHOLE.TXT */
void  LibraryDialog::Newlibrary()
{
		clearItems();
#ifndef Q_WS_QWS //sorry embedded gutenbrowser cant use zip files
   if ( newindexLib.open( IO_ReadOnly) ) {
      // file opened successfully
      setCaption( tr( "Library Index - using master pg index."  ) );
      QTextStream indexStream( &newindexLib );
      QString indexLine;
      while ( !indexStream.atEnd() )  { // until end of file..
         indexLine = indexStream.readLine();
         if ( ( indexLine.mid(4,4)).toInt() && !( indexLine.left(3)).toInt())  {
            year = indexLine.mid(4,4);
            file = indexLine.mid( indexLine.find( "[", 0, true )+1, 12 );
            number = indexLine.mid(  indexLine.find( "]", 0, true ) +1, indexLine.find( " ", 0, true )+1 );
            if( year.toInt() < 1984)
               number = number.left( number.length() -1 );
            title = indexLine.mid( indexLine.find(" ", 26, true), indexLine.length() );

						addItems();

         }// end if
      }// end while
      newindexLib.close();
   }
#ifndef Q_WS_QWS
   setCursor(  arrowCursor);
#endif
#endif
} // end Newlibrary()


void LibraryDialog::Library() {
   clearItems();

   IDontKnowWhy = "";
   system("date");
   if ( indexLib.open( IO_ReadOnly) ) {
      // file opened successfully
      QTextStream indexStream( &indexLib );
      QString indexLine;
      qApp->processEvents();

      bool okToRead = false;
      while ( !indexStream.eof() ) {
         indexLine = indexStream.readLine();
         if(indexLine == "<==Start GUTINDEX.ALL listings==>")
            okToRead = true;
         if(indexLine == "<==End of GUTINDEX.ALL==>") {
            okToRead = false;
            indexLib.at(indexLib.size());
         }

         if(okToRead) {
            QStringList token = QStringList::split(' ', indexLine);
            int textNumber;
            if(( textNumber = token.last().toInt() ))
               if(textNumber > 10001) {
                   // newer files with numbers > 100000 have new dir structure
                   // and need to be parsed differently..
                  if(textNumber < 10626)
                     year = "2003";
                  else if(textNumber >= 10626 && textNumber < 14600)
                     year = "2004";
                  else if(textNumber >= 14600)
                     year = "2005";

                  file = token.last();
                  title = indexLine.mid(0,72);

                  addItems(); //author and qlistview
               } else {
                   //end new etexts
                  if(token[1].toInt() && token[1].toInt() > 1969) {
                     year = token[1];
                     file = indexLine.mid(60,12);

                     if(file.left(1).find("[",0,TRUE) != -1) {
                        file.remove(1,1);
                        if( file.find("]",0,TRUE) != -1)
                           file = file.left( file.find("]",0,TRUE));

                        if(file.find("?", 0, false) != -1 ) {
                           QString tmpfile = file.replace(QRegExp("[?]"), "8");
                           file = tmpfile;
                        }
                     title = indexLine.mid( 9, 50);

                     addItems();
                  }
                  } else { // then try new format texts
                     file = token.last();
                     title = indexLine.mid(0,72);
                     year = "1980";

                     addItems(); //author and qlistview
                  }
               } //end old etexts

         } //end okToTRead
      }
      indexLib.close();
   } else {
      QString sMsg;

      sMsg = ( tr("<p>Error opening library index file. Please download a new one.</P> "));
      QMessageBox::message( "Error",sMsg);
   }
   system("date");
   sortLists(0);

} //end Library()


// Groks the author out of the title
bool LibraryDialog::getAuthor()
{
   if( title.contains( ", by", true)) {
      int auth;
      auth = title.find(", by", 0, true);
      author = title.right(title.length() - (auth + 4) );
      if( int finder = author.find("[", 0, true)) {
         author = author.left(finder);
      }
   }
   else if ( title.contains( "by, ", true) ) {
      int auth;
      auth = title.find("by, ", 0, true);
      author = title.right(title.length() - (auth + 4) );
      if( int finder = author.find("[", 0, true)) {
         author = author.left( finder);
      }
   }
   else if ( title.contains( " by", true) ) {
      int auth;
      auth = title.find(" by", 0, true);
      author = title.right(title.length() - (auth + 3) );
      if( int finder = author.find("[", 0, true)) {
         author = author.left( finder);
      }
   }
   else if ( title.contains( "by ", true) ) {
      int auth;
      auth = title.find("by ", 0, true);
      author = title.right(title.length() - (auth + 3) );
      if( int finder = author.find("[", 0, true)) {
         author = author.left( finder);
      }
   }
   else if ( title.contains( ",", true) ) {
      int auth;
      auth = title.find(",", 0, true);
      author = title.right( title.length() - (auth + 1) );
      if ( author.contains( ",", true) ) {
         int auth;
         auth = author.find(",", 0, true);
         author = author.right( author.length() - (auth + 1) );
      }
      if( int finder = author.find("[", 0, true)) {
         author = author.left( finder);
      }
   }
   else if ( title.contains( "/", true) ) {
      int auth;
      auth = title.find("/", 0, true);
      author = title.right(title.length() - (auth + 1) );
      if( int finder = author.find("[", 0, true)) {
         author = author.left( finder);
      }
   }
   else if ( title.contains( "of", true) ) {
      int auth;
      auth = title.find("of", 0, true);
      author = title.right(title.length() - (auth + 2) );
      if( int finder = author.find("[", 0, true))
      {
         author = author.left( finder);
      }
   } else {
      author = "";
   }
   if ( author.contains("et. al")) {
      int auth;
      auth = author.find("et. al", 0, true);
      author = author.left( auth );
   }
   if ( author.contains("#")) {
      int auth;
      auth = author.find("#", 0, true);
      author = author.left( auth);
   }
   if ( author.contains("(")) {
      int auth;
      auth = author.find("(", 0, true);
      author = author.left( auth);
   }
   if ( author.contains("et al")) {
      int auth;
      auth = author.find("et al", 0, true);
      author = author.left( auth );
   }
   QRegExp r = QRegExp("[0-9]", true, false);
   if ( author.left(2).find( r) != -1 ) {
			 author = "";
   }

	 author = author.stripWhiteSpace();
	 if (authBox->isChecked() == TRUE) {
             // this reverses the first name and last name of the author
             QString lastName, firstName="";
             int finder = author.findRev( ' ', -1, TRUE);
             lastName = author.right( author.length()-finder);
             firstName = author.left(finder);
             lastName = lastName.stripWhiteSpace();
             firstName = firstName.stripWhiteSpace();

             if( lastName.find( firstName, 0, true)  == -1)
                 // this avoids dup names
                 author = lastName+", "+firstName;
	 }
   return true;
}////// end getAuthor()

void LibraryDialog::addItems()
{
   cleanStrings();
   getAuthor();  // grok author

    etext etextStruct;
   if(  /*!number.isEmpty()
          && */
      (title.find( "reserved",0, FALSE) == -1)
       && (file.find( "]",0, true) == -1)
       &&(title.find( "Audio",0, FALSE) == -1)) {
      // fill string list or something to be able to sort by Author
       etextStruct.title = title;
       etextStruct.author = author;
       etextStruct.year = year;
       etextStruct.file = file;

      etextLibrary.append( etextStruct);

      if( author.isEmpty() )
         QList_Item5 = new QListViewItem( ListView5,  /*number, */author, title,  year, file );
      else  {
         if( author.find(QRegExp("[^a-fA-F]")) )
            QList_Item1 = new QListViewItem( ListView1, /* number,*/author,  title, year, file );

         else if(author.find(QRegExp("[^g-mG-M]")) )
            QList_Item2 = new QListViewItem( ListView2, /* number,*/ author, title,year, file );

         else if(author.find(QRegExp("[^n-rN-R]")) )
            QList_Item3 = new QListViewItem( ListView3, /* number,*/ author, title, year, file );

         else if(author.find(QRegExp("[^s-zS-Z]")) )
            QList_Item4 = new QListViewItem( ListView4, /* number,*/ author, title, year, file );
      }
   }
}

/*
  selected one etext*/
void LibraryDialog::select_title( QListViewItem * item)
{
   if(item != NULL) {
      i++;
      int index = tabWidget->currentPageIndex();
      DlglistItemTitle = item->text(0);
      DlglistItemYear = item->text(2);
      DlglistItemFile = item->text(3);

      switch (index) {
      case 0: {
         ListView1->clearSelection();
      }
         break;
      case 1: {
         ListView2->clearSelection();
      }
         break;
      case 2: {
         ListView3->clearSelection();
      }
         break;
      case 3: {
         ListView4->clearSelection();
      }
         break;
      case 4: {
         ListView5->clearSelection();
      }
         break;
      };
   }

   if(DlglistItemTitle.length() > 2) {
      item = 0;
      // todo check for connection here

      bool ok = false;
			qDebug(DlglistItemFile);

      if(	DlglistItemFile.toInt() > 10000 ) {
         // new directory sturcture
         if( download_newEtext())
            ok = true;
      } else {
         if(download_Etext())
            ok = true;
      }
      if(ok) {
         if(checkBox->isChecked () )
            accept();
      }
	 }
}

bool LibraryDialog::download_newEtext()
{ // ftp method
		QString fileName = DlglistItemFile;

    QString directory;
    int stringlength = DlglistItemFile.length();
    for(i = 0; i < stringlength - 1;  i++ ) {
				directory += "/"+ DlglistItemFile[i];
    }

		directory += "/" + DlglistItemFile;

    Config cfg("Gutenbrowser");
		cfg.setGroup("FTPsite");
		ftp_host = cfg.readEntry("SiteName", "sailor.gutenberg.org");
		ftp_base_dir = cfg.readEntry("base",  "/pub/gutenberg");

		if( ftp_base_dir.find("=",0,true) )
				ftp_base_dir.remove(  ftp_base_dir.find("=",0,true),1);

		QString dir = ftp_base_dir + directory;
		QString outputFile = local_library + ".guten_temp";
		QString file =  fileName + ".txt";

		QStringList networkList;
		networkList.append((const char *)ftp_host); //host
		networkList.append((const char *)dir); //ftp base directory
		networkList.append((const char *)outputFile); //output filepath
		networkList.append((const char *)file); //filename

		getEtext( networkList);

		return true;
}

bool LibraryDialog::getEtext(const QStringList &networkList)
{
    NetworkDialog *NetworkDlg;
    NetworkDlg = new NetworkDialog( this,"Network Protocol Dialog", true, 0, networkList);

    // use new, improved, *INSTANT* network-dialog-file-getterer
    if( NetworkDlg->exec() != 0 ) {
        File_Name = NetworkDlg->localFileName;

        qDebug("Just downloaded " + NetworkDlg->localFileName);

        if(NetworkDlg->successDownload) {
            if(File_Name.right(4) == ".txt") {
                QString  s_fileName = File_Name;
                s_fileName.replace( s_fileName.length() - 3, 3, "gtn");
                if (rename( File_Name.latin1(), s_fileName.latin1()) == 1) {
		    owarn << "Failed to rename " << File_Name.latin1() << " to "
			<< s_fileName.latin1() << oendl;
		    return false;
		}

                File_Name = s_fileName;
            }
            if(File_Name.length() > 5 ) {
                setTitle();
                QFileInfo fi(File_Name);
                QString  name_file = fi.fileName();
                name_file = name_file.left( name_file.length() - 4);

                DocLnk lnk;
                lnk.setName(name_file); //sets file name
                lnk.setComment(DlglistItemTitle);

                lnk.setFile(File_Name); //sets File property
                lnk.setType("guten/plain");// hey is this a REGISTERED mime type?!?!? ;D
                lnk.setExec(File_Name);
                lnk.setIcon("gutenbrowser/Gutenbrowser");
            } else
                QMessageBox::message("Note","<p>There was an error with the file</p>");
        }
    }

    return true;
}

bool LibraryDialog::download_Etext()
{ // ftp method
  // might have to use old gpl'd ftp for embedded!!

   Config cfg("Gutenbrowser");
   cfg.setGroup("FTPsite");
   ftp_host = cfg.readEntry("SiteName", "sailor.gutenberg.org");
   ftp_base_dir = cfg.readEntry("base",  "/pub/gutenberg");

  qDebug( "about to network dialog");

	QString NewlistItemNumber, NewlistItemYear, ls_result, result_line, s, dir, /*networkUrl, */outputFile;

   //////////////////// FIXME- if 'x' is part of real name....
   NewlistItemFile = DlglistItemFile.left(DlglistItemFile.find(".xxx", 1, false)).left(DlglistItemFile.left(DlglistItemFile.find(".xxx", 1, false)).find("x", 1, false));

   if( NewlistItemFile.find( DlglistItemFile.left(4) ,0,true) ==-1 ) {
      NewlistItemFile.replace( 0,4, DlglistItemFile.left(4));
			qDebug("NewlistItemFile is now " + NewlistItemFile);
   }

   NewlistItemYear = DlglistItemYear.right(2);
   int NewlistItemYear_Int = NewlistItemYear.toInt(0, 10);
   if (NewlistItemYear_Int < 91 && NewlistItemYear_Int > 70) {
      NewlistItemYear = "90";
   }

   Edir ="etext" +NewlistItemYear;

   dir = ftp_base_dir + "/etext"  + NewlistItemYear + "/";

   if( ftp_base_dir.find("=",0,true) )
      ftp_base_dir.remove(  ftp_base_dir.find("=",0,true),1);

   outputFile = local_library+".guten_temp";

	 qDebug( "Download file: " +NewlistItemFile);
	 qDebug("Checking: " + ftp_host + " " + dir + " " + outputFile+" " + NewlistItemFile);


   QStringList networkList;
   networkList.append((const char *)ftp_host); //host
   networkList.append((const char *)dir); //ftp base directory
   networkList.append((const char *)outputFile); //output filepath
   networkList.append((const char *)NewlistItemFile); //filename
   getEtext( networkList);

 return true;
}

bool LibraryDialog::httpDownload()
{//  httpDownload
#ifndef Q_WS_QWS
   Config config("Gutenbrowser");
   config.setGroup( "Browser" );
   QString brow = config.readEntry("Preferred", "");
   QString file_name = "./.guten_temp";
   QString httpName = proxy_http + "/"+Edir;
   i++;
   i++;
   QFile tmp( file_name);
   QString str;
   if (tmp.open(IO_ReadOnly))    {
      QTextStream t( &tmp );   // use a text stream
      while ( !t.eof())  {
         QString s = t.readLine();
         if (s.contains( NewlistItemFile, false) && (s.contains(".txt")) ) {
            str = s.mid( s.find( ".txt\">"+NewlistItemFile, 0, true)+6, (s.find( ".txt</A>", 0, true) + 4) - ( s.find( ".txt\">"+NewlistItemFile, 0, true)+6 ) );
            httpName += "/" + str;
         }
      }  //end of while loop
   }
   tmp.close();
   m_getFilePath = local_library + str;
   i++;
   if ( brow != "KFM"){ ///////// use lynx
      QString cmd = "lynx -source " + httpName +" | cat >> " + m_getFilePath;
      system(cmd);
   }
   i++;
#endif
   return false;
}

void LibraryDialog::cancelIt()
{
   saveConfig();

   DlglistItemNumber = "";
   this->reject();
}

bool LibraryDialog::setTitle()
{
   Config config("Gutenbrowser");

   if( DlglistItemTitle.find("[",0,true) != -1)
      DlglistItemTitle.replace(DlglistItemTitle.find("[",0,true),1, "(" );
   if( DlglistItemTitle.find("]",0,true) !=-1)
      DlglistItemTitle.replace(DlglistItemTitle.find("]",0,true),1, ")" );

   int test = 0;
   QString ramble, temp;
   config.setGroup("Files");
   QString s_numofFiles = config.readEntry("NumberOfFiles", "0" );
   int  i_numofFiles = s_numofFiles.toInt();
   for ( int i = 0; i <= i_numofFiles; i++){
      temp.setNum( i);
      ramble  = config.readEntry( temp, "" );
      if( strcmp( ramble, File_Name) == 0){
         test = 1;
      }
   }

   if(test == 0 ) {

      config.setGroup("Files");
      config.writeEntry( "NumberOfFiles", i_numofFiles +1 );
      QString interger;
      interger.setNum( i_numofFiles +1);
      config.writeEntry( interger, File_Name);
      config.setGroup( "Titles" );
      config.writeEntry( File_Name, DlglistItemTitle);
   }
   test = 0;
   return true;
}


void LibraryDialog::saveConfig()
{
   Config config("Gutenbrowser");
   if( httpBox->isChecked() == true) {
      checked = 1;
      config.setGroup( "Proxy" );
      config.writeEntry("IsChecked", "true");
   } else {
      checked = 0;
      config.setGroup( "Proxy" );
      config.writeEntry("IsChecked", "false");
   }
   if (authBox->isChecked() == true) {
      config.setGroup("SortAuth");
      config.writeEntry("authSort", "true");
   } else {
      config.setGroup("SortAuth");
      config.writeEntry("authSort", "false");
   }
}

/*
  searches library index for user word*/
void LibraryDialog::onButtonSearch()
{
   ListView1->clearSelection();
   ListView2->clearSelection();
   ListView3->clearSelection();
   ListView4->clearSelection();
   ListView5->clearSelection();

   int curTab = tabWidget->currentPageIndex();
   SearchDialog* searchDlg;

   //  if( resultsList)
   searchDlg = new SearchDialog( this, "Library Search", true);
   searchDlg->setCaption( tr( "Library Search"  ) );
   searchDlg->setLabel( "- author or title");

   QString resultString;

	 int i_berger = 0;
   if( searchDlg->exec() != 0 )  {
      QString searcherStr = searchDlg->get_text();

      bool cS;
      if( searchDlg->caseSensitiveCheckBox->isChecked())
         cS = true; //case sensitive
      else
         cS = false;

        etext etextStruct;
         QValueList<etext>::Iterator it;

        for( it = etextLibrary.begin(); it != etextLibrary.end(); ++it ) {
           QString tempTitle = (*it).title;
           QString tempAuthor = (*it).author;
           QString tempFile = (*it).file;
           QString tempYear = (*it).year;
           if(tempTitle.find( searcherStr, 0, cS) != -1
              || tempAuthor.find( searcherStr, 0, cS) != -1) {
              qWarning(tempTitle);
              Searchlist.append( tempTitle + " : " + tempYear + " : " + tempFile);
           }
        }


      tabWidget->setCurrentPage( curTab);

      Searchlist.sort();
      SearchResultsDlg* SearchResultsDialog;
      SearchResultsDialog = new SearchResultsDlg( searchDlg, "Results Dialog", true, 0 , Searchlist);

      SearchResultsDialog->showMaximized();
      if( SearchResultsDialog->exec() != 0) {
         texter = SearchResultsDialog->selText;
         resultLs = SearchResultsDialog->resultsList;
         i_berger = 1;
      } else {
         resultLs.clear();
      }
      Searchlist.clear();

      QString tester;
      for ( QStringList::Iterator it = resultLs.begin(); it != resultLs.end(); ++it ) {
         texter.sprintf("%s \n",(*it).latin1());
         if( tester!=texter)
            parseSearchResults( texter);
         tester = texter;
      }
      delete searchDlg;
   }
   if(checkBox->isChecked() ) {
      accept();
   } else {
      setActiveWindow();
   }
}

// splits the result string and calls download for the current search result
void  LibraryDialog::parseSearchResults( QString resultStr)
{
   int stringLeng = resultStr.length();

   QString my;
   my.setNum( stringLeng, 10);

    if( resultStr.length() > 2 && resultStr.length() < 130) {
        QStringList token = QStringList::split(" : ", resultStr);

        DlglistItemTitle  = token[0];
        DlglistItemTitle = DlglistItemTitle.stripWhiteSpace();

        DlglistItemYear  = token[1];
        DlglistItemYear = DlglistItemYear.stripWhiteSpace();

        DlglistItemFile = token[2];

        DlglistItemFile = DlglistItemFile.stripWhiteSpace();

        if(DlglistItemFile.left(1) == "/")
            DlglistItemFile =
                DlglistItemFile.right( DlglistItemFile.length() - 1);

        if(DlglistItemFile.toInt() > 10000 || DlglistItemYear == "1980" )
            // new directory sturcture
            download_newEtext();
        else
            download_Etext();
    }
}

void LibraryDialog::sortLists(int index)
{

    ListView1->setSorting(index);
    ListView2->setSorting(index);
    ListView3->setSorting(index);
    ListView4->setSorting(index);
    ListView5->setSorting(index);
    ListView1->sort();
    ListView2->sort();
    ListView3->sort();
    ListView4->sort();
    ListView5->sort();
}

// Downloads the current selected listitem
bool LibraryDialog::getItem(QListViewItem *it)
{
   DlglistItemTitle = it->text(0);
   DlglistItemYear = it->text(2);
   DlglistItemFile = it->text(3);

   if(download_Etext())  {
      if(i_binary == 1)  {
      }
   }
   return true;
}

// download button is pushed so we get the current items to download
bool LibraryDialog::onButtonDownload()
{
   QListViewItemIterator it1( ListView1 );
   QListViewItemIterator it2( ListView2 );
   QListViewItemIterator it3( ListView3 );
   QListViewItemIterator it4( ListView4 );
   QListViewItemIterator it5( ListView5 );

   // iterate through all items of the listview
   for ( ; it1.current(); ++it1 ) {
      if ( it1.current()->isSelected() )
         getItem(it1.current());
      it1.current()->setSelected(false);
   }
   for ( ; it2.current(); ++it2 ) {
      if ( it2.current()->isSelected() )
         getItem(it2.current());
      it2.current()->setSelected(false);
   }
   for ( ; it3.current(); ++it3 ) {
      if ( it3.current()->isSelected() )
         getItem(it3.current());
      it3.current()->setSelected(false);
   }
   for ( ; it4.current(); ++it4 ) {
      if ( it4.current()->isSelected() )
         getItem(it4.current());
      it4.current()->setSelected(false);
   }
   for ( ; it5.current(); ++it5 ) {
      if ( it5.current()->isSelected() )
         getItem(it5.current());
      it5.current()->setSelected(false);
   }
   return true;
}


// handles the sorting combo box
void LibraryDialog::comboSelect(int index)
{
   ListView1->setSorting( index, true);
   ListView2->setSorting( index, true);
   ListView3->setSorting( index, true);
   ListView4->setSorting( index, true);
   ListView5->setSorting( index, true);

   ListView1->sort();
   ListView2->sort();
   ListView3->sort();
   ListView4->sort();
   ListView5->sort();
}

void LibraryDialog::newList()
{
   if(indexLoaded) {
      onButtonDownload();
   } else {
      Output *outDlg;
      buttonNewList->setDown(true);
      QString gutenindex1 = local_library + "/GUTINDEX.ALL";

      QString cmd="wget -O " + gutenindex1 + " http://www.gutenberg.org/dirs/GUTINDEX.ALL 2>&1";

      int result = QMessageBox::warning( this,"Download"
                                         ,"<p>Ok to use /'wget/' to download a new library list?</P>"
                                         ,"Yes","No",0,0,1);
      qApp->processEvents();
      if(result == 0) {
         outDlg = new Output( 0, tr("Downloading Gutenberg Index...."),true);
         outDlg->showMaximized();
         outDlg->show();
         qApp->processEvents();
         FILE *fp;
         char line[130];
         outDlg->OutputEdit->append( tr("Running wget") );
         outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,false);
         sleep(1);
         fp = popen(  (const char *) cmd, "r");
         if ( fp ) {
            while ( fgets( line, sizeof line, fp)) {
               outDlg->OutputEdit->append(line);
               outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,false);
            }
            pclose(fp);
            outDlg->OutputEdit->append("Finished downloading\n");
            outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,false);
            qApp->processEvents();

         }
         FindLibrary();
         delete outDlg;
      }
      buttonNewList->setDown(false);
   }
}

bool LibraryDialog::moreInfo()
{

   QListViewItem * item;
   item = 0;
   QString titleString;
   item = ListView1->currentItem();
   if( item != 0) {
      titleString = item->text(0);
      ListView1->clearSelection();
      item = 0;
   }
   if( item == 0)
      item = ListView2->currentItem();
   if( item != 0) {
      titleString = item->text(0);
      ListView2->clearSelection();
      item = 0;
   }
   if( item == 0)
      item = ListView3->currentItem();
   if( item != 0) {
      titleString = item->text(0);
      ListView3->clearSelection();
      item = 0;
   }
   if( item == 0)
      item = ListView4->currentItem();
   if( item != 0) {
      titleString = item->text(0);
      ListView4->clearSelection();
      item = 0;
   }
   if( item == 0)
      item = ListView5->currentItem();
   if( item != 0) {
      titleString = item->text(0);
      ListView5->clearSelection();
      item = 0;
   }
   item=0;
   if(titleString.length()>2) {
      titleString.replace( QRegExp("\\s"), "%20");
      titleString.replace( QRegExp("'"), "%20");
      titleString.replace( QRegExp("\""), "%20");
      titleString.replace( QRegExp("&"), "%20");
      QString cmd= "http://google.com/search?q="+titleString+"&num=30&sa=Google+Search";
      cmd="opera "+cmd;
      system(cmd);
   } else
      QMessageBox::message( "Note","<p>If you select a title, this will search google.com for that title.</p>");
   return true;

}

// This loads the library Index
void LibraryDialog::FindLibrary()
{
   buttonLibrary->setDown(true);

   qApp->processEvents();
   if( QFile( new_index).exists()) {
      newindexLib.setName( new_index);
      indexLib.setName( new_index);
      Newlibrary();
   } else {
      newindexLib.setName( old_index);
      indexLib.setName( old_index);
      Library();
   }
   indexLoaded =true;
   buttonSearch->setEnabled(true);
   moreInfoButton->setEnabled(true);

   buttonLibrary->setDown(false);
   buttonNewList->setText("Download");
   qApp->processEvents();

}

void LibraryDialog::cleanStrings() {
   year = year.stripWhiteSpace();
   file = file.stripWhiteSpace();
   title = title.stripWhiteSpace();
   number = number.stripWhiteSpace();

}

void LibraryDialog::authBoxClicked()
{
   qApp->processEvents();
   FindLibrary();
}
