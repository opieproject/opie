/***************************************************************************
                          helpme.cpp  -  description
                             -------------------
    begin                : Tue Jul 25 2000
    begin                : Sat Dec 4 1999
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include "helpme.h"
#include "helpwindow.h"

#include <qprogressdialog.h>
#include <qlayout.h>

#include <sys/stat.h>
#include <unistd.h>

HelpMe::HelpMe(QWidget *parent,  QString name ) : QDialog(parent,name)
{
    local_library = ( QDir::homeDirPath ())+"/Applications/gutenbrowser/";
    setCaption(tr("Gutenbrowser About " VERSION));

  QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    Edit = new QMultiLineEdit(this, "");
    Edit->setReadOnly(true);
    Edit->append(tr("Based on gutenbook.pl, available from http://www.gutenbook.org"));
    Edit->append(tr("Much appreciation to Lee Burgess,"));
    Edit->append(tr("for the original idea and concept of gutenbook.\n"));
    Edit->append(tr("A list of current Gutenberg ftp sites is at http://promo.net/pg/index.html\n"));
    Edit->append(tr("For bug reports, comments or questions about Gutenbrowser, email"));
    Edit->append(tr("ljp@llornkcor.com\n"));
    Edit->append(tr("\nMade using Qt, Qt Embedded, and Qtopia, a cross platform development API\nhttp://www.trolltech.com\n\n"));
    Edit->append(tr("Arnold's Laws of Documentation:"));
    Edit->append(tr("(1) If it should exist, it doesn't."));
    Edit->append(tr("(2) If it does exist, it's out of date."));
    Edit->append(tr("(3) Only documentation for"));
    Edit->append(tr("useless programs transcends the"));
    Edit->append(tr("first two laws.\n"));
    Edit->append(tr("Everything is temporary, anyway....\n :o)"));
    Edit->setWordWrap(QMultiLineEdit::WidgetWidth);
  QString pixDir;
  pixDir=QPEApplication::qpeDir()+"/pics/";
    QPushButton *help;
    help = new QPushButton(this);
    help->setPixmap( QPixmap( pixDir+"gutenbrowser/help.png"));
    help->setText("Help");
    connect( help,SIGNAL(clicked()),this,SLOT( help() ));

/*    QPushButton *ok;
    ok = new QPushButton(this);
    ok->setPixmap(  QPixmap( pixDir+"/gutenbrowser/exit.png"));
    ok->setText("ok");
    connect(ok,SIGNAL(clicked()),this,SLOT(accept() ));
*/
    help->setFixedHeight(25);
//    ok->setFixedHeight(25);

//  layout->addMultiCellWidget( ok, 0, 0, 4, 4 );
  layout->addMultiCellWidget( help, 0, 0, 4, 4 );
  layout->addMultiCellWidget( Edit, 1, 1, 0, 4 );
}

HelpMe::~HelpMe()
{
//delete Edit;

}

void HelpMe::goToURL()
{

    url = "http://www.llornkcor.com/";
    goGetit( url);
}

void HelpMe::goToURL2()
{
    url = "http://www.gutenberg.org";
    goGetit( url);
}

void HelpMe::goToURL3()
{
    url = "http://www.gutenbook.org";
    goGetit( url);
}

void HelpMe::goGetit( QString url)
{
     HelpWindow *help = new HelpWindow( url, ".", 0, "gutenbrowser");
     help->setCaption("Qt Example - Helpviewer");
     help->showMaximized();
     help->show();
}

void HelpMe::help()
{
    QString msg ;
    msg=QPEApplication::qpeDir()+"help/html/gutenbrowser-index.html"; // or where ever this ends up to be
qDebug(msg);
    QString url = "file://"+msg;
    goGetit( url);

//       QCopEnvelope e("QPE/Application/helpbrowser", "setDocument(QString)" );
//       e << msg;
 
//    goGetit( msg);
}
