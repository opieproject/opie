/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (C) Holger Freyther <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
 
*/


/* OPIE */
#include <opie2/ofiledialog.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qfileinfo.h>
#include <qstring.h>
#include <qapplication.h>
#include <qlayout.h>

using namespace Opie::Ui;

namespace
{
/*
 * helper functions to load the start dir
 * and to save it
 * helper to extract the dir out of a file name
 */
/**
 * This method will use Config( argv[0] );
 * @param key The group key used
 */
QString lastUsedDir( const QString& key )
{
    if ( qApp->argc() < 1 )
        return QString::null;

    Config cfg( QFileInfo(qApp->argv()[0]).fileName() ); // appname
    cfg.setGroup( key );
    return cfg.readEntry("LastDir", QPEApplication::documentDir() );
}

void saveLastDir( const QString& key, const QString& file )
{
    if ( qApp->argc() < 1 )
        return;

    Config cfg(  QFileInfo(qApp->argv()[0]).fileName() );
    cfg.setGroup( key );
    QFileInfo inf( file );
    cfg.writeEntry("LastDir", inf.dirPath( true ) );
}
};

/**
 * This constructs a modal dialog
 *
 * @param caption The caption of the dialog
 * @param wid The parent widget
 * @param mode The mode of the OFileSelector @see OFileSelector
 * @param selector The selector of the OFileSelector
 * @param dirName the dir or resource to start from
 * @param fileName  a proposed or existing filename
 * @param mimetypes The mimeTypes
 */
OFileDialog::OFileDialog(const QString &caption,
                         QWidget *wid, int mode, int selector,
                         const QString &dirName,
                         const QString &fileName,
                         const QMap<QString,QStringList>& mimetypes )
        : QDialog( wid, "OFileDialog", true )
{
    //  QVBoxLayout *lay = new QVBoxLayout(this);
    //showMaximized();
    QVBoxLayout *lay = new QVBoxLayout(this );
    file = new OFileSelector(this , mode, selector,
                             dirName, fileName,
                             mimetypes );
    lay->addWidget( file );

    //lay->addWidget( file );
    //showFullScreen();
    setCaption( caption.isEmpty() ? tr("FileDialog") : caption );
    connect(file, SIGNAL(fileSelected(const QString&) ),
            this, SLOT(slotFileSelected(const QString&) ) );
    connect(file, SIGNAL(ok() ),
            this, SLOT(slotSelectorOk()) ) ;

    connect(file, SIGNAL(dirSelected(const QString&) ), this, SLOT(slotDirSelected(const QString&) ) );

#if 0
    connect(file, SIGNAL(dirSelected(const QString&) ),
            this, SLOT(slotDirSelected(const QString&) ) );
#endif
}
/**
 * @returns the mimetype of the selected
 * currently it return QString::null
 */
QString OFileDialog::mimetype()const
{
    return QString::null;
}

/**
 * @return the fileName
 */
QString OFileDialog::fileName()const
{
    return file->selectedName();
}

/**
 * return a DocLnk to the current file
 */
DocLnk OFileDialog::selectedDocument()const
{
    return file->selectedDocument();
}

/**
 * This opens up a filedialog in Open mode
 *
 * @param selector the Selector Mode
 * @param startDir Where to start from
 * @param file A proposed filename
 * @param mimes A list of MimeTypes
 * @param wid the parent
 * @param caption of the dialog if QString::null tr("Open") will be used
 * @return the fileName or QString::null
 */
QString OFileDialog::getOpenFileName(int selector,
                                     const QString &_startDir,
                                     const QString &file,
                                     const MimeTypes &mimes,
                                     QWidget *wid,
                                     const QString &caption )
{
    QString ret;
    QString startDir = _startDir;
    if (startDir.isEmpty() )
        startDir = lastUsedDir( "FileDialog-OPEN" );


    OFileDialog dlg( caption.isEmpty() ? tr("Open") : caption,
                     wid, OFileSelector::Open, selector, startDir, file, mimes);
    dlg.showMaximized();
    if( dlg.exec() )
    {
        ret = dlg.fileName();
        saveLastDir( "FileDialog-OPEN", ret );
    }

    return ret;
}

/**
 * This opens up a file dialog in save mode
 * @see getOpenFileName
 */
QString OFileDialog::getSaveFileName(int selector,
                                     const QString &_startDir,
                                     const QString &file,
                                     const MimeTypes &mimes,
                                     QWidget *wid,
                                     const QString &caption )
{
    QString ret;
    QString startDir = _startDir;
    if (startDir.isEmpty() )
        startDir = lastUsedDir( "FileDialog-SAVE" );

    OFileDialog dlg( caption.isEmpty() ? tr("Save") : caption,
                     wid, OFileSelector::Save, selector, startDir, file, mimes);
    dlg.showMaximized();
    if( dlg.exec() )
    {
        ret = dlg.fileName();
        saveLastDir( "FileDialog-SAVE", ret );
    }

    return ret;
}

void OFileDialog::slotFileSelected(const QString & )
{
    accept();
}

void OFileDialog::slotSelectorOk( )
{
    accept();
}

void OFileDialog::slotDirSelected(const QString &dir )
{
    setCaption( dir );
    // if mode
    //accept();
}
