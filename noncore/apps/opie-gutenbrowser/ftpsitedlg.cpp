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

#include <qpe/config.h>

/* QT */
#include <qprogressbar.h>
#include <qurloperator.h>
#include <qlistbox.h>

/* STD */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void optionsDialog::ftpSiteDlg( )
{
    local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
    ListFile = QPEApplication::qpeDir() + "etc/gutenbrowser";
    QDir dir(ListFile);
    if( !dir.exists())
        dir.mkdir(ListFile,true);
    ListFile+="/ftpList";
    qDebug( "opening "+ListFile );
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


// get ftp list from web- parse it.
void optionsDialog::getSite()
{
    QString file_name;
    QString ftp_listFileURL;
    QString outputFile;
    outputFile = ListFile + "list.html";
    QString networkUrl="http://www.gutenberg.net/catalog/world/selectpermanentmirror?fk_books=12962";

// TODO qprocess here

    QString cmd="wget -T 15 -O " +outputFile + " " + networkUrl;
 qDebug( "Issuing the command "+cmd );
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
    delete outDlg;

    ftp_QListBox_1->clear();
    parseFtpList( outputFile); // got the html list, now parse it so we can use it
}

bool optionsDialog::parseFtpList( QString outputFile)
{
// parse ftplist html and extract just the machine names/directories
// TODO: add locations!!
    qDebug( "parse ftplist "+outputFile );
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
            if( s.find( "Select a Default Download Site - Project Gutenberg", 0, TRUE) !=-1) { //lower end of this file
                b_gotchTest = true;
            }
            if( b_gotchTest) {
                if(( start = s.find( "ftp://", 0, TRUE))!=-1 ) {
                    end = s.find( "</td></tr>", 0, TRUE);// ==-1)) {
                    if( end == -1) {
                        end = s.find( "\">");
                    }
                    start =start + 6;
                    ftpSite =  s.mid( start , (end - start)  );

                    for (int j = 0 ; j<3;j++) {
                        s = t.readLine();
                        QString finder="</td><td rowspan=\"3\">";
                        if(( start = s.find( finder, 0, TRUE) ) != -1) {
                            end = s.find( finder, start + 2, TRUE) ;
                            s_location = s.mid( start + finder.length() , (end - start)-finder.length() );

                            ftpList += s_location+ "    "+ftpSite+"\n";
                            ftp_QListBox_1->sort( TRUE );
                            QString winbug="    ";
                            ftp_QListBox_1->insertItem ( s_location.latin1() + winbug + ftpSite);
                            j=3;
                        }
                    }
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

    qDebug( " just opens the ftp site list" );
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

// List box clicked
void optionsDialog::getSelection(  QListBoxItem *item)
{
    QString selctionStr;
    selctionStr = item->text();
    select_site( selctionStr );
}

void optionsDialog::select_site( const char *index )
{

    s_site = index;
    QString s_site2;
    s_site2=s_site.right( s_site.length()-(s_site.find("    ",0,TRUE)+4)  );

    qDebug( "Selected ftp site is "+ s_site2 );

    int i_ftp = s_site2.find("/", 0, FALSE);
    ftp_host = s_site2.left(i_ftp );
    ftp_base_dir = s_site2.right( s_site2.length() - i_ftp);

    Config config("Gutenbrowser");
    config.setGroup( "FTPsite" );
    config.writeEntry("SiteName",ftp_host);
    config.writeEntry("base",ftp_base_dir);
    TextLabel3->setText( "Current ftp server:\n" + ftp_host );
}

