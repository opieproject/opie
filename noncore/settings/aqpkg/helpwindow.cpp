/***************************************************************************
                          helpwindow.cpp  -  description
                             -------------------
    begin                : Sun Sep 8 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwidget.h>
#include <qlayout.h>
#include <qstring.h>
#include <qtextview.h>

#include "helpwindow.h"
#include "global.h"


#define HELP_TEXT \
"<qt><h1>Documentation for AQPkg</h1><p> " \
"AQPkg is a package manager for the Sharp Zaurus.<br><br> " \
"Basic Instructions:<br><br> " \
"On startup, you will be shown a window. The main part of the window is taken up " \
"by a list box showing packages. The packages shown will depend on the server selected.<br><br> " \
"The servers list contains network servers containing feeds of packages that can be downloaded  " \
"and installed onto your Zaurus. These are held in the file /etc/ipkg.conf and can be maintained  " \
"using AQPkg. In addition to the servers defined in ipkg.conf file, there are two other servers - " \
"local and local IPKGs. These are not network servers but views of yours Zaurus.<br><br> " \
"The local server shows all installed packages, and the local IPKGs server shows all ipks " \
"that are stored on your Zaurus.<br><br> " \
"On the local server, you can only remove packages. On the local IPKGs server you can only  " \
"install and delete packages - removing installed packages is currently not working. On all other  " \
"servers you can install, uninstall, upgrade and download packages.<br><br> " \
"To get the latest package list for a server (or refresh the view), select the server you " \
"wish to update and click the Refresh List button.<br><br> " \
"To download a package from a remote server, select the server (any except local and local IPKGs),  " \
"then select the package(s) you wish to download (by tapping in the box next to the package  " \
"name so that a tick appears in the box) and click the Download button. Enter the path where you  " \
"want the package to be downloaded to and click OK to download the package.<br><br> " \
"To install, upgrade or remove a package select the packages you wish to install and click the Apply " \
"button. You will then be shown a dialog which allows you to select which destination you wish " \
"to install the package to, which packages will be installed, removed and upgraded. You can also " \
"set various options. (for the moment, see the documentation for IPKG for more details on these " \
"options). To start the process, click Start. This will perform the necessary operations and " \
"will show you what is happening. Once everything has completed click the Close button.<br><br> " \
"Note: Currently, the operation to perform for a package is automatically decided based on the " \
"following rules:<br><br> " \
"   If a package isn't installed, then it will be installed.<br> " \
"   If a package is installed and there isn't a later version available then it will be removed.<br> " \
"   If a package is installed and a different version is available then it will be upgraded. (Note, " \
"I haven't yet found a way to determine if an available package is newer or older than the one " \
"currently installed so it is possible that a package may be downgraded).<br><br> " \
"As previously mentioned, a package can be explicitly removed by using the local server.<br><br> " \
"A couple of last notes, in the main window, the following may be useful:<br><br> " \
"If a package is installed then it will have (installed) after it.<br><br> " \
"If a different version is available then it will have a * after the package name.<br><br> " \
"You can view details of a package by tapping twice (quickly) on the package name (NOT the " \
"box next to the package name). This will show you a brief description of the package, the " \
"version installed (if it is installed), and the version available for download or installation " \
"(if a different on is available).<br><br> " \
"Well, hope you enjoy using this program. If you have any ideas/suggestions/ideas for improvements " \
"then please let me know at andy.qua@blueyonder.co.uk.<br><br> " \
"Thanks for using this. " \
"Andy. " \
"</p></qt>"


HelpWindow::HelpWindow( QWidget *parent, const char *name, bool modal, WFlags flags )
    : QDialog( parent, name, modal, flags )
{
//    resize( 230, 280 );

    setCaption( "Help for AQPkg" );

    QVBoxLayout *layout = new QVBoxLayout( this );
    QString text = HELP_TEXT;;
    QTextView *view = new QTextView( text, 0, this, "view" );
    layout->insertSpacing( -1, 5 );
    layout->insertWidget( -1, view );
    layout->insertSpacing( -1, 5 );

    showMaximized();
}

HelpWindow::~HelpWindow()
{
}
