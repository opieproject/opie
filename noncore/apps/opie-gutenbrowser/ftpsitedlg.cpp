/***************************************************************************
                          ftpsitedlg.cpp  -  description
                             -------------------
    begin                : Tue Jul 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "optionsDialog.h"
#include "gutenbrowser.h"
//#include "NetworkDialog.h"
#include "output.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>

/* QT */
#include <qprogressbar.h>
#include <qurloperator.h>
#include <qlistbox.h>

/* STD */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// :)~
void optionsDialog::ftpSiteDlg( )
{
//printf( "ftpSiteDlg: ListFile is "+ ListFile +"\n" );
//  initDialog();
    local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
//    ListFile = local_library + "ftpList";
    ListFile = QPEApplication::qpeDir() + "/etc/gutenbrowser";
    QDir dir(ListFile);
    if( !dir.exists())
        dir.mkdir(ListFile,true);
    ListFile+="/ftpList";
    odebug << "opening "+ListFile << oendl;
    if ( QFile(ListFile).exists() ) {
        openSiteList();
    } else {
           switch( QMessageBox::warning( this, "Gutenbrowser",
                                          "Do you want to  download \nan ftp site list?",
                                          QMessageBox::Yes,   QMessageBox::No)) {
                    case   QMessageBox::Yes: // yes
                        getSite();
                    break;
                    case   QMessageBox::No: // No
                      // exit
                    break;
            }

    }
}


/*
// get ftp list from web- parse it. */
void optionsDialog::getSite()
{
    QString file_name;
    QString ftp_listFileURL;
    QString outputFile;
//    outputFile = local_library+ "list.html";
    outputFile = ListFile + "list.html";
    QString networkUrl= "http://www.gutenberg.org/www/mirror.sites.html";
//http://www.gutenberg.org/index.html";
//       QString networkUrl= "http://llornkcor.com/index.shtml";
//    //  "http://www.gutenberg.org/index.html"
//
//Qhttp stops working at times.... :(
//    NetworkDialog *NetworkDlg;
//    NetworkDlg = new NetworkDialog( this,"Network Protocol Dialog",TRUE,0,networkUrl,outputFile);
//    if( NetworkDlg->exec() != 0 )
//    { // use new, improved, *INSTANT* network-dialog-file-getterer
//        odebug << "gitcha!" << oendl;
//    }
//    delete NetworkDlg;
//#ifdef Q_WS_QWS

// TODO qprocess here

    QString cmd="wget -T 15 -O " +outputFile + " " + networkUrl + " 2>&1" ;
 odebug << "Issuing the command "+cmd << oendl;
    Output *outDlg;
    outDlg = new Output( 0, tr("Downloading ftp sites...."),TRUE);
    outDlg->showMaximized();
    outDlg->show();
    qApp->processEvents();
    FILE *fp;
    char line[130];
    outDlg->OutputEdit->append( tr("Running wget") );
    sleep(1);
    fp = popen(  (const char *) cmd, "r");
    while ( fgets( line, sizeof line, fp)) {
        outDlg->OutputEdit->append(line);
        outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
    }
    pclose(fp);
    outDlg->close();
    if(outDlg)
        delete outDlg;

//    outputFile=ListFile;
    ftp_QListBox_1->clear();
    parseFtpList( outputFile); // got the html list, now parse it so we can use it
}

bool optionsDialog::parseFtpList( QString outputFile)
{
// parse ftplist html and extract just the machine names/directories
// TODO: add locations!!
    odebug << "parse ftplist "+outputFile << oendl;
    QString ftpList, s_location;
    QFile f( outputFile );

    if( f.open( IO_ReadWrite )) {
        QTextStream t( &f);
        QString countryName;
        bool b_gotchTest=false;

        while ( !t.eof() ) {
            QString s = t.readLine();
            int start;
            int end;
            if( s.find( "FTP mirror sites for Project Gutenberg:", 0, TRUE) !=-1) { //lower end of this file
                b_gotchTest = true;
            }
            if( b_gotchTest) {
                if(( start = s.find( "ftp://", 0, TRUE))!=-1 ) {
                    end = s.find( "/\"", 0, TRUE);// ==-1)) {
                    if( end == -1) {
                        end = s.find( "\">");
                    }
                    ftpSite =  s.mid( start, (end - start)  );
                    if(ftpSite.right(1) != "/") {
                          //    ftpSite += "/";
                    }
                    ftpSite=ftpSite.right( ftpSite.length()-6);
                    if( ftpSite.find("\n", 0, TRUE) )
                        ftpSite.remove( ftpSite.find("\n", 0, TRUE), 1);

                    if( ftpSite.find("\"", 0, TRUE) )
                        ftpSite.remove( ftpSite.find("\"", 0, TRUE), 1);
                    if( ftpSite.find("a>", 0, TRUE) )
                        ftpSite.remove( ftpSite.find("a>", 0, TRUE) -2, 4);


                    s = t.readLine();
                    s = t.readLine();
            if(( start=s.find("<BR>(", 0, TRUE) ) != -1) {
//              odebug << "" << s << "" << oendl;
              end = s.find( ")", 0, TRUE)+1;
              s_location= s.mid( start+4, (end - start)  );

//               odebug << "" << s_location << "" << oendl;
//                    ftpList += ftpSite + "\n";
//                    ftp_QListBox_1->sort( TRUE );
                    ftpList += s_location+ "    "+ftpSite+"\n";

                    ftp_QListBox_1->sort( TRUE );
                    QString winbug="    ";
                    ftp_QListBox_1->insertItem ( s_location.latin1() +winbug+ftpSite);
                    // ftp_QListBox_1->insertItem ( ftpSite+"  "+s_location.latin1());
            }

                    //  ftp_QListBox_1->insertItem ( ftpSite);
                }
            } // end find ftp://

        } // end while loop
        QFile f2( ListFile);
    if(!f2.open( IO_ReadWrite | IO_Truncate))
        QMessageBox::message( (tr("Note")), (tr("File not opened sucessfully.\n"+ListFile )) );
        f2.writeBlock( ftpList, ftpList.length() );
        f.close();
        f2.close();
       if( f.exists() )
        f.remove();
    } else {
        QMessageBox::message( (tr("ftpSiteDlg")), (tr("List File not opened sucessfully." )) );
        return false;
    }
    setActiveWindow();
    return true;
}

void optionsDialog::openSiteList() {

    odebug << " just opens the ftp site list" << oendl;
//  ListFile = ( QDir::homeDirPath ()) +"/.gutenbrowser/ftpList";
    QFile f( ListFile);
    if(!f.open( IO_ReadWrite )) {
        QMessageBox::message( (tr("Note")), (tr("File not opened sucessfully." )) );
    } else {
        QTextStream t( &f);
        while ( !t.atEnd() ) {
            QString ftpSite = t.readLine();
            ftp_QListBox_1->sort( TRUE );
            ftp_QListBox_1->insertItem ( ftpSite);

        } // end while loop
        f.close();
    }

    QListBoxItem *itemSel=0;
    if( (itemSel=ftp_QListBox_1->findItem( ftp_host)) )
        ftp_QListBox_1->setSelected( itemSel, true);
}

/*
List box clicked */
void optionsDialog::getSelection(  QListBoxItem *item)
{
    QString selctionStr;
    selctionStr = item->text();
//  selctionStr = ftp_QListBox_1->currentText();
//  printf( selctionStr+"\n" );
    select_site( selctionStr );
}

void optionsDialog::select_site( const char *index )
{
//   ftp://ftp.datacanyon.com/pub/gutenberg/

    s_site = index;
    QString s_site2;
//    if(s_site.find("(",0,TRUE))
    s_site2=s_site.right( s_site.length()-(s_site.find("    ",0,TRUE)+4)  );
odebug << "Selected ftp site is "+ s_site2 << oendl;
    int i_ftp = s_site2.find("/", 0, FALSE);
    ftp_host = s_site2.left(i_ftp );
    ftp_base_dir = s_site2.right( s_site2.length() - i_ftp);

//    config->read();
    Config config("Gutenbrowser");
    config.setGroup( "FTPsite" );
    config.writeEntry("SiteName",ftp_host);
    config.writeEntry("base",ftp_base_dir);
//    config->write();
    TextLabel3->setText( "Current ftp server:\n"+ftp_host /*+ ftp_base_dir*/ );
//    optionsDialog::accept();
}

