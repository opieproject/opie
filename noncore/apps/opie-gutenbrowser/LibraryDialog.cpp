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

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpedialog.h>

#include "LibraryDialog.h"
#include "output.h"

#include <qpushbutton.h>
#include <qmultilineedit.h>
//#include <qlayout.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog. */
LibraryDialog::LibraryDialog( QWidget* parent,  const char* name , bool modal, WFlags fl )
				: QDialog( parent, name, true/* modal*/, fl )
{
		if ( !name )
				setName( "LibraryDialog" );
		indexLoaded=false;
		initDialog();

      //      this->setMaximumWidth(240);

		index = "GUTINDEX.ALL";
		local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
		local_index = local_library + index;

		QString iniFile ;
		iniFile = QPEApplication::qpeDir()+"/etc/gutenbrowser/gutenbrowserrc";

		new_index =QPEApplication::qpeDir()+"/etc/gutenbrowser/PGWHOLE.TXT";

		old_index = QPEApplication::qpeDir()+"/etc/gutenbrowser/GUTINDEX.ALL";
			//    old_index = QPEApplication::qpeDir()+"etc/gutenbrowser/GUTINDEX.ALL";

			//     iniFile = local_library+"gutenbrowserrc";
			//     new_index = local_library + "PGWHOLE.TXT";
			//     old_index = local_library + "GUTINDEX.ALL";

		Config config("Gutenbrowser");

		config.setGroup( "HttpServer" );
		proxy_http = config.readEntry("Preferred", "http://sailor.gutenbook.org");

		config.setGroup( "FTPsite" );
		ftp_host=config.readEntry("SiteName", "sailor.gutenberg.org");
		qDebug("Library Dialog: ftp_host is "+ftp_host);
			//      ftp_host=ftp_host.right(ftp_host.length()-(ftp_host.find(") ",0,TRUE)+1) );
			//      ftp_host=ftp_host.stripWhiteSpace();
		ftp_base_dir= config.readEntry("base",  "/pub/gutenberg");

		i_binary = 0;

		config.setGroup("SortAuth");
		if( config.readEntry("authSort", "FALSE") == "TRUE")
				authBox->setChecked(TRUE);

		config.setGroup("General");
		downDir =config.readEntry( "DownloadDirectory",local_library);
		qDebug("downDir is "+downDir);
		newindexLib.setName( old_index);
		indexLib.setName( old_index);

		new QPEDialogListener(this);    
}

LibraryDialog::~LibraryDialog()
{
// 		delete QList_Item2;
// 		delete QList_Item1;
// 		delete QList_Item3;
// 		delete QList_Item4;
// 		delete QList_Item5;

	//	saveConfig();
}

  /*This groks using PGWHOLE.TXT */
void  LibraryDialog::Newlibrary()
{
#ifndef Q_WS_QWS //sorry embedded gutenbrowser cant use zip files
			//qDebug("Opening new library index %s",newindexLib);
		if ( newindexLib.open( IO_ReadOnly) ) {
				setCaption( tr( "Library Index - using master pg index."  ) );// file opened successfully
				QTextStream indexStream( &newindexLib );
				QString indexLine;
				while ( !indexStream.atEnd() )  { // until end of file..
						indexLine = indexStream.readLine();
						if ( ( indexLine.mid(4,4)).toInt() && !( indexLine.left(3)).toInt())  {
								year = indexLine.mid(4,4);
								year = year.stripWhiteSpace();
								file = indexLine.mid( indexLine.find( "[", 0, TRUE )+1, 12 );
								file = file.stripWhiteSpace();
								number = indexLine.mid(  indexLine.find( "]", 0, TRUE ) +1, indexLine.find( " ", 0, TRUE )+1 );
								if( year.toInt() < 1984)
										number = number.left( number.length() -1 );
								number = number.stripWhiteSpace();
								title = indexLine.mid( indexLine.find(" ", 26, TRUE), indexLine.length() );
								title = title.stripWhiteSpace();

								getAuthor();  // groks author
								author = author.stripWhiteSpace();
								if (authBox->isChecked()) { // this reverses the first name and last name of the author
											//                    qDebug("Sorting last name first");
										QString lastName, firstName="";
										int finder=author.findRev( ' ', -1, TRUE);
										lastName=author.right( author.length()-finder);
										firstName=author.left(finder);
										lastName=lastName.stripWhiteSpace();
										firstName=firstName.stripWhiteSpace();

										if( lastName.find( firstName, 0, true)  == -1) // this avoids dup names
												author=lastName+", "+firstName;
								}

								if( !number.isEmpty() && (title.find( "reserved",0, FALSE) == -1) && (file.find( "]",0, TRUE) == -1) ) {

											// fill string list or something to be able to resort the whole library
										if( author.isEmpty() )
												QList_Item5 = new QListViewItem( ListView5, /* number,*/ title, author, year, file );
										else {

												if(  (author.left(1) >= QString("A") && author.left(1) <= QString("F")) ||
														 (author.left(1) >= QString("a")  && author.left(1) <= QString("f")) )
														QList_Item1 = new QListViewItem( ListView1,/*  number,*/ title, author, year, file );

												else if( (author.left(1) >= QString("G") && author.left(1) <= QString("M")) ||
																 (author.left(1) >= QString("g") && author.left(1) <= QString("m")) )
														QList_Item2 = new QListViewItem( ListView2,  /*number, */title, author, year, file );

												else if( (author.left(1) >= QString("N") && author.left(1) <= QString("R")) ||
																 (author.left(1) >= QString("n") && author.left(1) <= QString("r")) )
														QList_Item3 = new QListViewItem( ListView3,  /*number,*/ title, author, year, file );

												else if( (author.left(1) >= QString("S") && author.left(1) <= QString("Z")) ||
																 (author.left(1) >= QString("s") && author.left(1) <= QString("z")) )
														QList_Item4 = new QListViewItem( ListView4, /* number,*/ title, author, year, file );

												else
														QList_Item5 = new QListViewItem( ListView5, /* number,*/ title, author, year, file );
										}
								}
						}// end if
				}// end while
				newindexLib.close();
		}
#ifndef Q_WS_QWS
		setCursor(  arrowCursor);
#endif
#endif
} // end Newlibrary()


void LibraryDialog::Library()
{// old library groking method

		ListView1->clear();
		ListView2->clear();
		ListView3->clear();
		ListView4->clear();
		ListView5->clear();

		qDebug("opening GUTINDEX.ALL file");
		IDontKnowWhy = "";
		if ( indexLib.open( IO_ReadOnly) ) {  // file opened successfully
				QTextStream indexStream( &indexLib );
				QString indexLine;
				qApp->processEvents();
					//        int jig;
				while ( !indexStream.eof() ) {

						indexLine = indexStream.readLine();
						if ( indexLine != "") {
								if( (indexLine.mid(4,4)).toInt() /* && !( indexLine.left(3)).toInt()*/ ) {
											//                    month = indexLine.left( 3);
										year = indexLine.mid(4,4);
											//                    title = indexLine.mid( 9, 50);
										file = indexLine.mid(60,12);
										if(file.left(1).find("[",0,TRUE) != -1)
												file.remove(1,1);
										if( file.find("]",0,TRUE) != -1)
												file = file.left( file.find("]",0,TRUE));
											//qDebug("file is "+file);
											///               number = indexLine.mid(  indexLine.find( "]", 0, TRUE ) +1, indexLine.find( " ", 0, TRUE )+1 );
										number = indexLine.mid(55,5);
										number = number.stripWhiteSpace();
											//                  title = indexLine.mid( indexLine.find(" ", 26, TRUE), indexLine.length() );
										title = indexLine.mid( 9,  50 );
										title = title.stripWhiteSpace();
											//qDebug("title is "+title);
										getAuthor();  // grok author
										author = author.stripWhiteSpace();
											//qDebug("author is "+author);
										if (authBox->isChecked() == TRUE) { // this reverses the first name and last name of the author
												QString lastName, firstName="";
												int finder=author.findRev( ' ', -1, TRUE);
												lastName=author.right( author.length()-finder);
												firstName=author.left(finder);
												lastName=lastName.stripWhiteSpace();
												firstName=firstName.stripWhiteSpace();

												if( lastName.find( firstName, 0, true)  == -1) // this avoids dup names
														author=lastName+", "+firstName;
										}

										if( !number.isEmpty() && (title.find( "reserved",0, FALSE) == -1) /*&& (file.find( "]",0, TRUE))*/ ) {
													// fill string list or something to be able to sort by Author
												if( author.isEmpty() )
														QList_Item5 = new QListViewItem( ListView5,  /*number, */title, author, year, file );
												else  {
														if(  (author.left(1) >= QString("A") && author.left(1) <= QString("F")) ||
																 (author.left(1) >= QString("a")  && author.left(1) <= QString("f")) )
																QList_Item1 = new QListViewItem( ListView1, /* number,*/ title, author, year, file );

														else if( (author.left(1) >= QString("G") && author.left(1) <= QString("M")) ||
																		 (author.left(1) >= QString("g") && author.left(1) <= QString("m")) )
																QList_Item2 = new QListViewItem( ListView2, /* number,*/ title, author, year, file );

														else if( (author.left(1) >= QString("N") && author.left(1) <= QString("R")) ||
																		 (author.left(1) >= QString("n") && author.left(1) <= QString("r")) )
																QList_Item3 = new QListViewItem( ListView3, /* number,*/ title, author, year, file );

														else if( (author.left(1) >= QString("S") && author.left(1) <= QString("Z")) ||
																		 (author.left(1) >= QString("s") && author.left(1) <= QString("z")) )
																QList_Item4 = new QListViewItem( ListView4, /* number,*/ title, author, year, file );
												}
										}
								}
						}
				}
				indexLib.close();
		} else {
				QString sMsg;
				sMsg = ( tr("Error opening local library index:\n "))+local_index;
				QMessageBox::message( "Error",sMsg);
		}

} //end Library()


  /*
    Groks the author out of the title */
bool LibraryDialog::getAuthor()
{
		if( title.contains( ", by", TRUE)) {
				int auth;
				auth = title.find(", by", 0, TRUE);
				author = title.right(title.length() - (auth + 4) );
				if( int finder = author.find("[", 0, TRUE)) {
						author = author.left(finder);
				}
		}
		else if ( title.contains( "by, ", TRUE) ) {
				int auth;
				auth = title.find("by, ", 0, TRUE);
				author = title.right(title.length() - (auth + 4) );
				if( int finder = author.find("[", 0, TRUE)) {
						author = author.left( finder);
				}
		}
		else if ( title.contains( " by", TRUE) ) {
				int auth;
				auth = title.find(" by", 0, TRUE);
				author = title.right(title.length() - (auth + 3) );
				if( int finder = author.find("[", 0, TRUE)) {
						author = author.left( finder);
				}
		}
		else if ( title.contains( "by ", TRUE) ) {
				int auth;
				auth = title.find("by ", 0, TRUE);
				author = title.right(title.length() - (auth + 3) );
				if( int finder = author.find("[", 0, TRUE)) {
						author = author.left( finder);
				}
		}
		else if ( title.contains( ",", TRUE) ) {
				int auth;
				auth = title.find(",", 0, TRUE);
				author = title.right( title.length() - (auth + 1) );
				if ( author.contains( ",", TRUE) ) {
						int auth;
						auth = author.find(",", 0, TRUE);
						author = author.right( author.length() - (auth + 1) );
				}
				if( int finder = author.find("[", 0, TRUE)) {
						author = author.left( finder);
				}
		}
		else if ( title.contains( "/", TRUE) ) {
				int auth;
				auth = title.find("/", 0, TRUE);
				author = title.right(title.length() - (auth + 1) );
				if( int finder = author.find("[", 0, TRUE)) {
						author = author.left( finder);
				}
		}
		else if ( title.contains( "of", TRUE) ) {
				int auth;
				auth = title.find("of", 0, TRUE);
				author = title.right(title.length() - (auth + 2) );
				if( int finder = author.find("[", 0, TRUE))
					{
							author = author.left( finder);
					}
		} else {
				author = "";
		}
		if ( author.contains("et. al")) {
				int auth;
				auth = author.find("et. al", 0, TRUE);
				author = author.left( auth );
		}
		if ( author.contains("#")) {
				int auth;
				auth = author.find("#", 0, TRUE);
				author = author.left( auth);
		}
		if ( author.contains("(")) {
				int auth;
				auth = author.find("(", 0, TRUE);
				author = author.left( auth);
		}
		if ( author.contains("et al")) {
				int auth;
				auth = author.find("et al", 0, TRUE);
				author = author.left( auth );
		}
		QRegExp r = QRegExp("[0-9]", TRUE, FALSE);
		if ( author.left(2).find( r) != -1 ) {
				author = "";
		}
			// if( author.contains(" ", TRUE)) {
			// int suth = author.findRev(" ", -1, TRUE);
			// author = author.right( author.length() - suth);
			// }

			// title
			// author
		return true;
}////// end getAuthor()

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

		if(DlglistItemTitle.length()>2) {
					//    DlglistItemNumber = item->text(0);
				item = 0;
				qDebug( "string from librarydialog is:%s %s %s", DlglistItemYear.latin1(),DlglistItemFile.latin1(),DlglistItemNumber.latin1());
				qDebug("Title is "+DlglistItemTitle);

					// check for connection here
					//    if( get_extext())

				if(download_Etext())  {
							//        qDebug("get here 2");
						if(i_binary == 1)  {

						}
						if(checkBox->isChecked () ) {
								accept();
						}
				}
		}
}

bool  LibraryDialog::download_Etext()
{ // ftp method
  // might have to use old gpl'd ftp for embedded!!
		Config cfg("Gutenbrowser");
		cfg.setGroup("FTPsite");
		ftp_host=cfg.readEntry("SiteName", "sailor.gutenberg.org");
		ftp_base_dir= cfg.readEntry("base",  "/pub/gutenberg");

		qDebug("about to network dialog");
		QString NewlistItemNumber, NewlistItemYear, ls_result, result_line, s, dir, networkUrl, outputFile;

			//////////////////// FIXME- if 'x' is part of real name....
		NewlistItemFile = DlglistItemFile.left(DlglistItemFile.find(".xxx", 1, FALSE)).left(DlglistItemFile.left(DlglistItemFile.find(".xxx", 1, FALSE)).find("x", 1, FALSE));

		if( NewlistItemFile.find( DlglistItemFile.left(4) ,0,TRUE) ==-1 ) {
				NewlistItemFile.replace( 0,4, DlglistItemFile.left(4));
				qDebug("NewlistItemFile is now "+NewlistItemFile);
		}
		NewlistItemYear = DlglistItemYear.right(2);
		int NewlistItemYear_Int = NewlistItemYear.toInt(0, 10);
		qDebug(NewlistItemYear);
		if (NewlistItemYear_Int < 91 && NewlistItemYear_Int > 70) {
				NewlistItemYear = "90";
		}
		Edir ="etext" +NewlistItemYear;
		dir= ftp_base_dir + "/etext" +NewlistItemYear+"/";
		if( ftp_base_dir.find("=",0,true) )
				ftp_base_dir.remove(  ftp_base_dir.find("=",0,true),1);

		networkUrl= "ftp://"+ftp_host+dir;

		outputFile=local_library+".guten_temp";
			//qDebug("Download file:%s",NewlistItemFile.latin1() );
		qDebug("Checking: "+ftp_host+" "+dir+" "+outputFile+" "+NewlistItemFile);
		QStringList networkList;
		networkList.append((const char *)ftp_host);
		networkList.append((const char *)dir);
		networkList.append((const char *)outputFile); 
		networkList.append((const char *)NewlistItemFile);
//<< (char *)ftp_host << (char *)dir << (char *)outputFile << (char *)NewlistItemFile;

		NetworkDialog *NetworkDlg;
		NetworkDlg = new NetworkDialog( this,"Network Protocol Dialog", TRUE, 0, networkList);

		
		if( NetworkDlg->exec() != 0 )  { // use new, improved, *INSTANT* network-dialog-file-getterer
				File_Name= NetworkDlg->localFileName;
				qDebug("back to Library from Network Dialog");
				qDebug("Just downloaded "+NetworkDlg->localFileName);

					//          if (File_Name.find( local_library, 0, TRUE) != -1 ) { //could not be found
					//              QString oldName=File_Name;
					//              File_Name.replace(0,local_library.length(),downDir);
					//              qDebug("File_Name now is "+File_Name);

					//          }
					// rename .txt to .etx
				if(NetworkDlg->successDownload) {
						qDebug("Filename is "+File_Name);
						if(File_Name.right(4)==".txt") {
								QString  s_fileName=File_Name;
								s_fileName.replace( s_fileName.length()-3,3,"gtn");
									//                s_fileName.replace( s_fileName.length()-3,3,"etx");
								rename(File_Name.latin1(),s_fileName.latin1());
								File_Name=s_fileName;

								qDebug("Filename is now "+File_Name);

						}
						if(File_Name.length() > 5 ) {
								setTitle();
								QFileInfo fi(File_Name);
								QString  name_file=fi.fileName();
								name_file=name_file.left(name_file.length()-4);

								qDebug("Setting doclink");
								DocLnk lnk;
								qDebug("name is "+name_file);
								lnk.setName(name_file); //sets file name
								qDebug("Title is "+DlglistItemTitle);
								lnk.setComment(DlglistItemTitle);

								qDebug("Filename is "+File_Name);
								lnk.setFile(File_Name); //sets File property
								lnk.setType("guten/plain");// hey is this a REGISTERED mime type?!?!? ;D
								lnk.setExec(File_Name);
								lnk.setIcon("gutenbrowser/Gutenbrowser");
								if(!lnk.writeLink()) {
										qDebug("Writing doclink did not work");
								} else {
								}
						} else
								QMessageBox::message("Note","There was an error\nwith the file");
				}
		}
		return true;
}

bool LibraryDialog::httpDownload()
{//  httpDownload
#ifndef Q_WS_QWS
		Config config("Gutenbrowser");
		config.setGroup( "Browser" );
		QString brow = config.readEntry("Preferred", "");
		QString file_name = "./.guten_temp";
			//    config.setGroup( "HttpServer" );
			//    QString s_http = config.readEntry("Preferred", "http://sailor.gutenbook.org");
		QString httpName = proxy_http + "/"+Edir;
			//    progressBar->setProgress( i);
		i++;
		if ( brow != "Konq")    { /////////// use lynx
					//        QString cmd = "lynx -source " + httpName +" | cat >> " + file_name;
					//        system(cmd);
		}    else    { //////////// use KFM
					//        KFM::download( httpName, file_name);
		}
		i++;
		QFile tmp( file_name);
		QString str;
		if (tmp.open(IO_ReadOnly))    {
				QTextStream t( &tmp );   // use a text stream
				while ( !t.eof())  {
						QString s = t.readLine();
						if (s.contains( NewlistItemFile, FALSE) && (s.contains(".txt")) ) {
								str = s.mid( s.find( ".txt\">"+NewlistItemFile, 0, TRUE)+6, (s.find( ".txt</A>", 0, TRUE) + 4) - ( s.find( ".txt\">"+NewlistItemFile, 0, TRUE)+6 ) );
								httpName += "/" + str;
						}
				}  //end of while loop
		}
		tmp.close();
		m_getFilePath = local_library + str;
		i++;
		if ( brow != "KFM"){ ///////// use lynx
				QString cmd = "lynx -source " + httpName +" | cat >> " + m_getFilePath;
					//        QMessageBox::message("Error", cmd);
				system(cmd);
		} else { ////////// use KFM
					//        KFM::download( httpName, m_getFilePath);
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
		qDebug("setting title");
		qDebug(DlglistItemTitle);

		if( DlglistItemTitle.find("[",0,TRUE) != -1)
				DlglistItemTitle.replace(DlglistItemTitle.find("[",0,TRUE),1, "(" );
		if( DlglistItemTitle.find("]",0,TRUE) !=-1)
				DlglistItemTitle.replace(DlglistItemTitle.find("]",0,TRUE),1, ")" );
		qDebug("Title being set is "+DlglistItemTitle);
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
		config.setGroup("Files");
		config.writeEntry( "NumberOfFiles", i_numofFiles +1 );
		QString interger;
		interger.setNum( i_numofFiles +1);
		config.writeEntry( interger, File_Name);
		config.setGroup( "Titles" );
		config.writeEntry( File_Name, DlglistItemTitle);

		test = 0;
		return true;
}


void LibraryDialog::saveConfig()
{
		Config config("Gutenbrowser");
		if( httpBox->isChecked() == TRUE) {
				checked = 1;
				config.setGroup( "Proxy" );
				config.writeEntry("IsChecked", "TRUE");
		} else {
				checked = 0;
				config.setGroup( "Proxy" );
				config.writeEntry("IsChecked", "FALSE");
		}
		if (authBox->isChecked() == TRUE) {
				config.setGroup("SortAuth");
				config.writeEntry("authSort", "TRUE");
		} else {
				config.setGroup("SortAuth");
				config.writeEntry("authSort", "FALSE");
		}
			//    config.write();
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

		int curTab=tabWidget->currentPageIndex();
		SearchDialog* searchDlg;

			//  if( resultsList)
		searchDlg = new SearchDialog( this, "Library Search", TRUE);
		searchDlg->setCaption( tr( "Library Search"  ) );
		searchDlg->setLabel( "- author or title");
		QString resultString;
		int i_berger = 0;
		if( searchDlg->exec() != 0 )  {
				QString searcherStr = searchDlg->get_text();
				int fluff=0;

					//        int tabPage = tabWidget->currentPageIndex();
					// TODO ititerate here... struct<listViews>??

				QListViewItemIterator it1( ListView1 );
				QListViewItemIterator it2( ListView2 );
				QListViewItemIterator it3( ListView3 );
				QListViewItemIterator it4( ListView4 );
				QListViewItemIterator it5( ListView5 );

					//// this is really pitiful work,
					///////
				bool cS;
				if( searchDlg->caseSensitiveCheckBox->isChecked())
						cS=true; //case sensitive
				else
						cS=false;

				if(fluff==0) {
						for ( ; it1.current(); ++it1 ) {
								resultString = ( it1.current() )->text(0);
								resultString += (" : ");
								resultString += ( it1.current() )->text(2);
								resultString += (" : ");
								resultString += ( it1.current() )->text(3);
								if( resultString.find( searcherStr, 0, cS) != -1)
									{
											Searchlist.append( resultString);
									}
						}
				}
				if(fluff==0) {// search routine here
						for ( ; it2.current(); ++it2 ) {
								resultString = ( it2.current() )->text(0);
								resultString += (" : ");
								resultString += ( it2.current() )->text(2);
								resultString += (" : ");
								resultString += ( it2.current() )->text(3);
								if( resultString.find( searcherStr, 0, cS) != -1) {
										Searchlist.append( resultString);
								}
						}
				}
				if(fluff==0) {// search routine here
						for ( ; it3.current(); ++it3 ) {
								resultString = ( it3.current() )->text(0);
								resultString += (" : ");
								resultString += ( it3.current() )->text(2);
								resultString += (" : ");
								resultString += ( it3.current() )->text(3);

								if( resultString.find( searcherStr, 0, cS) != -1) {
										Searchlist.append( resultString);
								}
						}
				}
				if(fluff==0) {
							// search routine here
						for ( ; it4.current(); ++it4 )  {
								resultString = ( it4.current() )->text(0);
								resultString += (" : ");
								resultString += ( it4.current() )->text(2);
								resultString += (" : ");
								resultString += ( it4.current() )->text(3);
								if( resultString.find( searcherStr, 0, cS) != -1) {
										Searchlist.append( resultString);
								}
						}
				}
				if(fluff==0) {              // search routine here
						for ( ; it5.current(); ++it5 )  {
								resultString = ( it5.current() )->text(0);
								resultString += (" : ");
								resultString += ( it5.current() )->text(2);
								resultString += (" : ");
								resultString += ( it5.current() )->text(3);
								if( resultString.find( searcherStr, 0, cS) != -1) {
										Searchlist.append( resultString);
								}
						}
				}

				tabWidget->setCurrentPage( curTab);

				Searchlist.sort();
				SearchResultsDlg* SearchResultsDialog;
				SearchResultsDialog = new SearchResultsDlg( searchDlg, "Results Dialog", true, 0 , Searchlist);

				SearchResultsDialog->showMaximized();
				if( SearchResultsDialog->exec() != 0) {
						texter = SearchResultsDialog->selText;
							//           qDebug(texter);
						resultLs= SearchResultsDialog->resultsList;
						i_berger = 1;
				}
				Searchlist.clear();

					//        if(SearchResultsDialog)
					//        delete SearchResultsDialog;
				QString tester;
				for ( QStringList::Iterator it = resultLs.begin(); it != resultLs.end(); ++it ) {
						texter.sprintf("%s \n",(*it).latin1());
							//           qDebug(texter);
						if( tester!=texter)
								parseSearchResults( texter);
						tester = texter;
				}
				if(searchDlg)
						delete searchDlg;
		}
		if(checkBox->isChecked() ) {
				accept();
		} else {
				setActiveWindow();
		}
}

  /*
    splits the result string and calls download for the current search result*/
void  LibraryDialog::parseSearchResults( QString resultStr)
{

		int stringLeng=resultStr.length();
		QString my;
		my.setNum( stringLeng, 10);

		if( resultStr.length() > 2 && resultStr.length() < 130) {
				int titleInt = resultStr.find( " : ", 0, TRUE);
				DlglistItemTitle  =   resultStr.left( titleInt);
				int yearInt = resultStr.find( " : ", titleInt+3, TRUE);
				DlglistItemYear  = resultStr.mid( titleInt+3, (yearInt - titleInt)-3);
				DlglistItemFile = resultStr.right( resultStr.length() - (yearInt + 3));
				download_Etext();
		}
			/*
				printf( DlglistItemTitle+"\n");  printf( DlglistItemYear+"\n");  printf( DlglistItemFile+"\n");*/
}

  //  bool LibraryDialog::UnzipIt( QString zipFile) {
  //  //////////TODO findsome other way of dealingwithzip files.
  //        ///usr/bin/unzip";
  //      if( QFile::exists( zipFile))        {
  //  //  QString thatFile = local_library +"PGWHOLE.TXT";
  //          QString cmd;
  //  #if defined(_WS_X11_)
  //        cmd = "gunzip -d " + zipFile /*newestLibraryFile  */+" -d " + local_library;
  //  #endif
  //  #if defined(_WS_WIN_)
  //        QString temp= QDir::convertSeparators(local_library);
  //        zipFile=QDir::convertSeparators( zipFile);
  //        cmd = temp+"unzip.exe -o " +zipFile/*newestLibraryFile  */+" -d " + temp;
  //  #endif
  //  #ifndef Q_WS_QWS
  //  //      QString cmd = "gunzip -d " + zipFile /*newestLibraryFile  */+" -d " + local_library;
  //        cmd = "unzip " + zipFile;
  //  #endif

  //          int exit=QMessageBox::information(this, "Unzip?", "Ok to unzip "+ zipFile+" ?", QMessageBox::Yes, QMessageBox::No);
  //          if (exit==QMessageBox::Yes) {
  //              qDebug("Issuing the command "+cmd);
  //  #if defined(_WS_WIN_)
  //      WinExec( cmd, SW_HIDE );
  //  #endif
  //  #if defined(_WS_X11_)
  //      system( cmd);
  //  #endif
  //  #ifndef Q_WS_QWS
  //      system( cmd);
  //  #endif
  //  // printf("unzip\n");
  //        //     remove( zipFile /*newestLibraryFile*/);
  //              return true;
  //          }
  //          else if(exit==QMessageBox::No)  {
  //          //    printf("unzip\n");
  //              return false;
  //          }
  //      } else  {
  //  //          QMessageBox::message( "Note",( tr("Please install unzip in your PATH")) );
  //          return false;
  //      }
  //      return true;
  //  }

void LibraryDialog::sort()
{

}

  /*
    Downloads the current selected listitem*/
bool LibraryDialog::getItem(QListViewItem *it)
{
			//    qDebug("selected getItem");

			//    DlglistItemNumber = it->text(0);
		DlglistItemTitle = it->text(0);
		DlglistItemYear = it->text(2);
		DlglistItemFile = it->text(3);

		if(download_Etext())  {
				if(i_binary == 1)  {
				}
		}
		return true;
}

  /*
    download button is pushed so we get the current items to download*/
bool LibraryDialog::onButtonDownload()
{
			//    qDebug("selected onButtonDownloadz");

		QListViewItemIterator it1( ListView1 );
		QListViewItemIterator it2( ListView2 );
		QListViewItemIterator it3( ListView3 );
		QListViewItemIterator it4( ListView4 );
		QListViewItemIterator it5( ListView5 );

			// iterate through all items of the listview
		for ( ; it1.current(); ++it1 ) {
				if ( it1.current()->isSelected() )
						getItem(it1.current());
				it1.current()->setSelected(FALSE);
		}
		for ( ; it2.current(); ++it2 ) {
				if ( it2.current()->isSelected() )
						getItem(it2.current());
				it2.current()->setSelected(FALSE);
		}
		for ( ; it3.current(); ++it3 ) {
				if ( it3.current()->isSelected() )
						getItem(it3.current());
				it3.current()->setSelected(FALSE);
		}
		for ( ; it4.current(); ++it4 ) {
				if ( it4.current()->isSelected() )
						getItem(it4.current());
				it4.current()->setSelected(FALSE);
		}
		for ( ; it5.current(); ++it5 ) {
				if ( it5.current()->isSelected() )
						getItem(it5.current());
				it5.current()->setSelected(FALSE);
		}
		return true;
}


  /*
    handles the sorting combo box */
void LibraryDialog::comboSelect(int index)
{
			//    qDebug("we are sorting");
		ListView1->setSorting( index, TRUE);
		ListView2->setSorting( index, TRUE);
		ListView3->setSorting( index, TRUE);
		ListView4->setSorting( index, TRUE);
		ListView5->setSorting( index, TRUE);

		ListView1->sort();
		ListView2->sort();
		ListView3->sort();
		ListView4->sort();
		ListView5->sort();

			//      ListView1->triggerUpdate();
			//      ListView2->triggerUpdate();
			//      ListView3->triggerUpdate();
			//      ListView4->triggerUpdate();
			//      ListView5->triggerUpdate();
}

void LibraryDialog::newList()
{
		if(indexLoaded) {
				onButtonDownload();
		} else {
				Output *outDlg;
				buttonNewList->setDown(TRUE);
				QDir gutDir(QPEApplication::qpeDir()+"etc/gutenbrowser");
				if(!gutDir.exists()) gutDir.mkdir(QPEApplication::qpeDir()+"etc/gutenbrowser",true);
				if( chdir(QPEApplication::qpeDir()+"etc/gutenbrowser") == 0) {
						qDebug("changing dir "+QPEApplication::qpeDir()+"etc/gutenbrowser");
						QString gutenindex1 = QPEApplication::qpeDir()+"etc/gutenbrowser/GUTINDEX.ALL";
						QString cmd="wget -O " + gutenindex1 + " http://sailor.gutenberg.org/GUTINDEX.ALL 2>&1";

						int result = QMessageBox::warning( this,"Download"
																							 ,"Ok to use /'wget/' to download\na new library list?\n"
																							 ,"Yes","No",0,0,1);
						qApp->processEvents();
						if(result == 0) {
								outDlg = new Output( 0, tr("Downloading Gutenberg Index...."),TRUE);
								outDlg->showMaximized();
								outDlg->show();
								qApp->processEvents();
								FILE *fp;
								char line[130];
								outDlg->OutputEdit->append( tr("Running wget") );
								outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
								sleep(1);
								fp = popen(  (const char *) cmd, "r");
								if ( !fp ) {
								} else {
										qDebug("Issuing the command\n"+cmd);
											//                 system(cmd);
										while ( fgets( line, sizeof line, fp)) {
												outDlg->OutputEdit->append(line);
												outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
										}
										pclose(fp);
										outDlg->OutputEdit->append("Finished downloading\n");
										outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
										qApp->processEvents();

											//                  if( QFile(gutenindex1).exists() ) {
											//                      QString gutenindex=QPEApplication::qpeDir()+"etc/gutenbrowser/GUTINDEX.ALL";
											//                      if( rename(gutenindex1.latin1(),gutenindex.latin1()) !=0)
											//                          qDebug("renaming error");
											//                  }

								}
									//               outDlg->close();
								FindLibrary();
								if(outDlg) delete outDlg;
						}
						buttonNewList->setDown(FALSE);
				} else {
						QMessageBox::message("Note","Could not change directories");
				}
					//         if(outDlg)
					//             delete outDlg;
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
				qDebug( "Title is "+titleString );
				titleString.replace( QRegExp("\\s"), "%20");
				titleString.replace( QRegExp("'"), "%20");
				titleString.replace( QRegExp("\""), "%20");
				titleString.replace( QRegExp("&"), "%20");
				QString cmd= "http://google.com/search?q="+titleString+"&num=30&sa=Google+Search";
				cmd="opera "+cmd;
				system(cmd);
		} else
				QMessageBox::message( "Note","If you select a title, this will\nsearch google.com for that title.");
		return true;

}

  /*
    This loads the library Index*/
void LibraryDialog::FindLibrary()
{
		buttonLibrary->setDown(TRUE);

		qApp->processEvents();
		if( QFile( new_index).exists() /* && this->isHidden() */) {
				newindexLib.setName( new_index);
				indexLib.setName( new_index);
				qDebug("index file is "+ new_index);
				Newlibrary();
		} else {
				newindexLib.setName( old_index);
				indexLib.setName( old_index);
				qDebug("new index nameis "+ old_index);
				Library();
		}
		indexLoaded=true;
		buttonSearch->setEnabled(TRUE);
		moreInfoButton->setEnabled(TRUE);

		buttonLibrary->setDown(FALSE);
		buttonNewList->setText("Download");
		qApp->processEvents();

}
