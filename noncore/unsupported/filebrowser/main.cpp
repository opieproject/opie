/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "filebrowser.h"

#include <qpe/qpeapplication.h>
#include <qpe/mimetype.h>

#include <qmainwindow.h>
#include <qfileinfo.h>
#include <qdir.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    MimeType::updateApplications();
    FileBrowser mw( QDir::current().canonicalPath() );
    mw.resize( 240, 320 );
    mw.setCaption( FileBrowser::tr("File Manager") );
    a.showMainWidget( &mw );

    return a.exec();
}
