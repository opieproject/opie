/**********************************************************************
// textedit.cpp
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
**********************************************************************/
// changes added by L. J. Potter Sun 02-17-2002 21:31:31

#include "textedit.h"
#include "filePermissions.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/ofileselector.h>
#include <opie2/ofiledialog.h>
#include <opie2/ofontselector.h>
#include <opie2/oresource.h>

#include <qpe/config.h>
#include <qpe/qpeapplication.h>


/* QT */
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtextstream.h>
#include <qclipboard.h>
#include <qaction.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qdir.h>

/* STD */
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h> //getenv

using namespace Opie::Core;
using namespace Opie::Ui;

#if QT_VERSION < 0x030000
class QpeEditor : public QMultiLineEdit
{

public:
    QpeEditor( QWidget *parent, const char * name = 0 )
            : QMultiLineEdit( parent, name ) {
        clearTableFlags();
        setTableFlags( Tbl_vScrollBar | Tbl_autoHScrollBar );
    }

    void find( const QString &txt, bool caseSensitive,
                            bool backwards );
protected:
    bool markIt;
    int line1, line2, col1, col2;
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );

//public slots:
      /*
signals:
    void notFound();
    void searchWrapped();
    */

private:

};

void QpeEditor::mousePressEvent( QMouseEvent *e )
{
    switch(e->button()) {
    case RightButton:
        { //rediculous workaround for qt popup menu
            //and the hold right click mechanism
            this->setSelection( line1, col1, line2, col2);
            QMultiLineEdit::mousePressEvent( e );
            markIt = false;
        }
        break;
    default:
        {
            if(!markIt) {
                int line, col;
                this->getCursorPosition(&line, &col);
                line1=line2=line;
                col1=col2=col;
            }
            QMultiLineEdit::mousePressEvent( e );
        }
        break;
    };
}

void QpeEditor::mouseReleaseEvent( QMouseEvent * )
{
    if(this->hasMarkedText()) {
        markIt = true;
        this->getMarkedRegion( &line1, &col1, &line2, & col2 );
    }
    else {
        markIt = false;
    }
}

void QpeEditor::find ( const QString &txt, bool caseSensitive,
           bool backwards )
{
    static bool wrap = false;
    int line, col;
    if ( wrap ) {
        if ( !backwards )
            line = col = 0;
        wrap = false;
          //  emit searchWrapped();
    }
    else {
        getCursorPosition( &line, &col );
    }
      //ignore backwards for now....
    if ( !backwards ) {
        for ( ; ; ) {
            if ( line >= numLines() ) {
                wrap = true;
                  //emit notFound();
                break;
            }
            int findCol = getString( line )->find( txt, col, caseSensitive );
            if ( findCol >= 0 ) {
                setCursorPosition( line, findCol, false );
                col = findCol + txt.length();
                setCursorPosition( line, col, true );

                  //found = true;
                break;
            }
            line++;
            col = 0;
        }
    }
}


#else

#error "Must make a QpeEditor that inherits QTextEdit"

#endif


static const int nfontsizes = 6;
static const int fontsize[nfontsizes] = {8,10,12,14,18,24};

TextEdit::TextEdit( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f )
{
    doc = 0;
    edited=false;

    setToolBarsMovable( false );
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    channel = new QCopChannel( "QPE/Application/textedit", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
        this, SLOT(receive(const QCString&,const QByteArray&)) );

    setIcon( Opie::Core::OResource::loadPixmap( "textedit/TextEditor", Opie::Core::OResource::SmallIcon ) );

    QToolBar *bar = new QToolBar( this );
    bar->setHorizontalStretchable( true );
    menu = bar;

    QMenuBar *mb = new QMenuBar( bar );
    QPopupMenu *file = new QPopupMenu( this );
    QPopupMenu *edit = new QPopupMenu( this );
    QPopupMenu *advancedMenu = new QPopupMenu(this);

    font = new QPopupMenu( this );

    bar = new QToolBar( this );
    editBar = bar;

    QAction *a = new QAction( tr( "New" ), Opie::Core::OResource::loadPixmap( "new", Opie::Core::OResource::SmallIcon ),
                              QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
//    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Open" ), Opie::Core::OResource::loadPixmap( "fileopen", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Save" ), Opie::Core::OResource::loadPixmap( "save", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( save() ) );
    file->insertSeparator();
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Save As" ),  Opie::Core::OResource::loadPixmap( "save", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( saveAs() ) );
    a->addTo( file );

    file->insertSeparator();
    deleteAction = new QAction( tr( "Delete" ), Opie::Core::OResource::loadPixmap( "close", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( deleteAction, SIGNAL( activated() ), this, SLOT( fileDelete() ) );
    deleteAction->addTo( file );

    undoAction = new QAction( tr( "Undo" ), Opie::Core::OResource::loadPixmap( "undo", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    undoAction->addTo( edit );
    undoAction->setEnabled( false );

    redoAction = new QAction( tr( "Redo" ), Opie::Core::OResource::loadPixmap( "redo", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    redoAction->addTo( edit );
    redoAction->setEnabled( false );

    edit->insertSeparator();

    a = new QAction( tr( "Cut" ), Opie::Core::OResource::loadPixmap( "cut", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Copy" ), Opie::Core::OResource::loadPixmap( "copy", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Paste" ), Opie::Core::OResource::loadPixmap( "paste", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( editBar );
    a->addTo( edit );


#ifndef QT_NO_CLIPBOARD
    a = new QAction( tr( "Insert Time and Date" ), Opie::Core::OResource::loadPixmap( "paste", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPasteTimeDate() ) );
    a->addTo( edit );
#endif

    a = new QAction( tr( "Goto Line..." ), Opie::Core::OResource::loadPixmap( "find", Opie::Core::OResource::SmallIcon ),
                    QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( gotoLine() ) );
    edit->insertSeparator();
    a->addTo( edit );

    a = new QAction( tr( "Find..." ), Opie::Core::OResource::loadPixmap( "find", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editFind() ) );
    a->addTo( bar );
    a->addTo( edit );

    zin = new QAction( tr("Zoom in"), QString::null, 0, this, 0 );
    connect( zin, SIGNAL( activated() ), this, SLOT( zoomIn() ) );
    zin->addTo( font );

    zout = new QAction( tr("Zoom out"), QString::null, 0, this, 0 );
    connect( zout, SIGNAL( activated() ), this, SLOT( zoomOut() ) );
    zout->addTo( font );

    font->insertSeparator();

    font->insertItem(tr("Font"), this, SLOT(changeFont()) );

    font->insertSeparator();
    font->insertItem(tr("Advanced Features"), advancedMenu);

    QAction *wa = new QAction( tr("Wrap lines"),
                               QString::null, 0, this, 0 );
    connect( wa, SIGNAL( toggled(bool) ),
             this, SLOT( setWordWrap(bool) ) );
    wa->setToggleAction(true);
    wa->addTo( advancedMenu);

    desktopAction = new QAction( tr("Always open linked file"),
                                 QString::null, 0, this, 0 );
    connect( desktopAction, SIGNAL( toggled(bool) ),
             this, SLOT( doDesktop(bool) ) );
    desktopAction->setToggleAction(true);
    desktopAction->addTo( advancedMenu);

    filePermAction = new QAction( tr("File Permissions"),
                                  QString::null, 0, this, 0 );
    connect( filePermAction, SIGNAL( toggled(bool) ),
             this, SLOT( doFilePerms(bool) ) );
    filePermAction->setToggleAction(true);
    filePermAction->addTo( advancedMenu);

    nAutoSave = new QAction( tr("Auto Save 5 min."),
                                   QString::null, 0, this, 0 );
    connect( nAutoSave, SIGNAL( toggled(bool) ),
             this, SLOT( doTimer(bool) ) );
    nAutoSave->setToggleAction(true);
    nAutoSave->addTo( advancedMenu);

    mb->insertItem( tr( "File" ), file );
    mb->insertItem( tr( "Edit" ), edit );
    mb->insertItem( tr( "View" ), font );

    searchBar = new QToolBar(this);
    addToolBar( searchBar,  "Search", QMainWindow::Top, true );

    searchBar->setHorizontalStretchable( true );

    searchEdit = new QLineEdit( searchBar, "searchEdit" );
    searchBar->setStretchableWidget( searchEdit );
    connect( searchEdit, SIGNAL( textChanged(const QString&) ),
       this, SLOT( search() ) );

    a = new QAction( tr( "Find Next" ), Opie::Core::OResource::loadPixmap( "next", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( findNext() ) );
    a->addTo( searchBar );
    a->addTo( edit );

    a = new QAction( tr( "Close Find" ), Opie::Core::OResource::loadPixmap( "close", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
    a->addTo( searchBar );

    searchBar->hide();

    editor = new QpeEditor( this );
    setCentralWidget( editor );
    editor->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    connect( editor, SIGNAL( textChanged() ),
             this, SLOT( editorChanged() ) );
    connect( editor, SIGNAL( undoAvailable(bool) ),
             undoAction, SLOT( setEnabled(bool) ) );
    connect( editor, SIGNAL( redoAvailable(bool) ),
             redoAction, SLOT( setEnabled(bool) ) );
    connect( undoAction, SIGNAL( activated() ), editor, SLOT( undo() ) );
    connect( redoAction, SIGNAL( activated() ), editor, SLOT( redo() ) );

    QPEApplication::setStylusOperation( editor, QPEApplication::RightOnHold);

    Config cfg("TextEdit");
    cfg. setGroup ( "Font" );

    QFont defaultFont = editor-> font ( );

    QString family = cfg. readEntry     ( "Family", defaultFont. family ( ));
    int size       = cfg. readNumEntry  ( "Size",   defaultFont. pointSize ( ));
    int weight     = cfg. readNumEntry  ( "Weight", defaultFont. weight ( ));
    bool italic    = cfg. readBoolEntry ( "Italic", defaultFont. italic ( ));

    defaultFont = QFont ( family, size, weight, italic );
    editor-> setFont ( defaultFont );

    cfg.setGroup ( "View" );

    openDesktop = cfg.readBoolEntry ( "OpenDesktop", true );
    filePerms = cfg.readBoolEntry ( "FilePermissions", false );
    featureAutoSave = cfg.readBoolEntry( "autosave", false);

    if(openDesktop) desktopAction->setOn( true );
    if(filePerms) filePermAction->setOn( true );
    if(featureAutoSave) nAutoSave->setOn(true);

//       {
//           doTimer(true);
//       }

    bool wrap = cfg. readBoolEntry ( "Wrap", true );
    wa-> setOn ( wrap );
    setWordWrap ( wrap );

/////////////////
    if( qApp->argc() > 1) {
        currentFileName=qApp->argv()[1];

        QFileInfo fi(currentFileName);

        if(fi.baseName().left(1) == "") {
            openDotFile(currentFileName);
        }
        else {
            openFile(currentFileName);
        }
    }
    else {
        newFile(DocLnk());
    }
}

TextEdit::~TextEdit()
{
    delete editor;
}

void TextEdit::closeEvent(QCloseEvent *e)
{
    if (savePrompt())
        e->accept();
    else
        e->ignore();
}

void TextEdit::cleanUp()
{
    Config cfg ( "TextEdit" );
    cfg. setGroup ( "Font" );
    QFont f = editor->font();
    cfg.writeEntry ( "Family", f. family ( ));
    cfg.writeEntry ( "Size",   f. pointSize ( ));
    cfg.writeEntry ( "Weight", f. weight ( ));
    cfg.writeEntry ( "Italic", f. italic ( ));

    cfg.setGroup ( "View" );
    cfg.writeEntry ( "Wrap", editor->wordWrap() == QMultiLineEdit::WidgetWidth );

    cfg.writeEntry ( "OpenDesktop", openDesktop );
    cfg.writeEntry ( "FilePermissions", filePerms );
}

void TextEdit::accept()
{
    if( edited ) {
        if(save())
            qApp->quit();
    }
    else
        qApp->quit();
}

void TextEdit::zoomIn()
{
    setFontSize(editor->font().pointSize()+1,false);
}

void TextEdit::zoomOut()
{
    setFontSize(editor->font().pointSize()-1,true);
}


void TextEdit::setFontSize(int sz, bool round_down_not_up)
{
    int s=10;
    for (int i=0; i<nfontsizes; i++) {
        if ( fontsize[i] == sz ) {
            s = sz;
            break;
        }
        else if ( round_down_not_up ) {
            if ( fontsize[i] < sz )
                s = fontsize[i];
        }
        else {
            if ( fontsize[i] > sz ) {
                s = fontsize[i];
                break;
            }
        }
    }

    QFont f = editor->font();
    f.setPointSize(s);
    editor->setFont(f);

    zin->setEnabled(s != fontsize[nfontsizes-1]);
    zout->setEnabled(s != fontsize[0]);
}

void TextEdit::setWordWrap(bool y)
{
    bool state = editor->edited();
    QString captionStr = caption();
    bool b = edited;

    editor->setWordWrap(y ? QMultiLineEdit::WidgetWidth : QMultiLineEdit::NoWrap );
    editor->setEdited( state );
    edited=b;
    setCaption(captionStr);
}

void TextEdit::fileNew()
{
    if(savePrompt())
        newFile(DocLnk());
}

void TextEdit::fileOpen()
{
    if(!savePrompt())
        return;

    Config cfg("TextEdit");
    cfg. setGroup ( "View" );
    QMap<QString, QStringList> map;
    map.insert(tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr("Text"), text );
    text << "*";
    map.insert(tr("All"), text );
    QString str = OFileDialog::getOpenFileName( 2,
                                                QString::null ,
                                                QString::null, map);
    if( !str.isEmpty() && QFile(str).exists() && !QFileInfo(str).isDir() )
       openFile( str );
}

#if 0
void TextEdit::slotFind()
{
    FindDialog frmFind( tr("Text Editor"), this );
    connect( &frmFind, SIGNAL(signalFindClicked(const QString&,bool,bool,int)),
       editor, SLOT(slotDoFind(const QString&,bool,bool)));

    //case sensitive, backwards, [category]

    connect( editor, SIGNAL(notFound()),
       &frmFind, SLOT(slotNotFound()) );
    connect( editor, SIGNAL(searchWrapped()),
       &frmFind, SLOT(slotWrapAround()) );

    frmFind.exec();


}
#endif

void TextEdit::editCut()
{
#ifndef QT_NO_CLIPBOARD
    editor->cut();
#endif
}

void TextEdit::editCopy()
{
#ifndef QT_NO_CLIPBOARD
    editor->copy();
#endif
}

void TextEdit::editPaste()
{
#ifndef QT_NO_CLIPBOARD
    editor->paste();
#endif
}

void TextEdit::editFind()
{
    searchBar->show();
    searchEdit->setFocus();
}

void TextEdit::findNext()
{
    editor->find( searchEdit->text(), false, false );
}

void TextEdit::findClose()
{
    searchBar->hide();
}

void TextEdit::search()
{
    editor->find( searchEdit->text(), false, false );
}

void TextEdit::newFile( const DocLnk &f ) {
    DocLnk nf = f;
    nf.setType("text/plain");
    clear();
    setWState (WState_Reserved1 );
    editor->setFocus();
    doc = new DocLnk(nf);
    currentFileName = "";
    updateCaption();
    resetEditStatus();
    checkEnableDelete();
}

void TextEdit::openDotFile( const QString &f )
{
    if(!currentFileName.isEmpty()) {
        currentFileName=f;

        odebug << "openFile dotfile " + currentFileName << oendl;
        QString txt;
        QFile file(f);
        if (!file.open(IO_ReadWrite))
            owarn << "Failed to open file " << file.name() << oendl;
        else {
            QTextStream t(&file);
            while ( !t.atEnd()) {
                txt+=t.readLine()+"\n";
            }
            editor->setText(txt);
            resetEditStatus();
            checkEnableDelete();
        }
    }
    updateCaption();
}

void TextEdit::openFile( const QString &f )
{
    odebug << "filename is "+ f << oendl;
    QString filer;
    QFileInfo fi( f);
    if(f.find(".desktop",0,true) != -1 && !openDesktop ) {
        switch ( QMessageBox::warning(this,tr("Text Editor"),tr("Text Editor has detected<BR>you selected a <B>.desktop</B>file.<BR>Open<B>.desktop</B> file or <B>linked</B> file?"),tr(".desktop File"),tr("Linked Document"),0,1,1) ) {
        case 0: //desktop
            filer = f;
            break;
        case 1: //linked
            DocLnk sf(f);
            filer = sf.file();
            break;
        };
    }
    else if(fi.baseName().left(1) == "") {
        odebug << "opening dotfile" << oendl;
        currentFileName=f;
        openDotFile(currentFileName);
        return;
    }
    /*
     * The problem is a file where Config(f).isValid() and it does not
     * end with .desktop will be treated as desktop file
     */
    else if (f.find(".desktop",0,true) != -1 ) {
        DocLnk sf(f);
        filer = sf.file();
        if(filer.right(1) == "/")
            filer = f;
    }
    else
        filer = f;

    DocLnk nf;
    nf.setType("text/plain");
    nf.setFile(filer);
    currentFileName=filer;

    nf.setName(fi.baseName());
    openFile(nf);

    odebug << "openFile string "+currentFileName << oendl;

    showEditTools();
    // Show filename in caption
    updateCaption();
    checkEnableDelete();
}

void TextEdit::openFile( const DocLnk &f )
{
//    clear();
    FileManager fm;
    QString txt;
    currentFileName=f.file();
    odebug << "openFile doclnk " + currentFileName << oendl;
    if ( !fm.loadFile( f, txt ) ) {
          // ####### could be a new file
        odebug << "Cannot open file" << oendl;
    }
    if ( doc )
        delete doc;
    doc = new DocLnk(f);
    editor->setText(txt);
    resetEditStatus();

    doc->setName(currentFileName);
    updateCaption();
    setTimer();
    checkEnableDelete();
}

void TextEdit::showEditTools() {
    menu->show();
    editBar->show();
    setWState (WState_Reserved1 );
}

void TextEdit::resetEditStatus() {
    editor->setEdited( false);
    edited=false;
    // Clear undo buffer
    editor->setUndoEnabled(false);
    editor->setUndoEnabled(true);
}

void TextEdit::checkEnableDelete() {
    if( doc && currentFileName != "" && doc->file().left(5) != "/etc/" ) 
        deleteAction->setEnabled(true);
    else
        deleteAction->setEnabled(false);
}

/*!
  unprompted save */
bool TextEdit::save()
{
    QString name, file;
    odebug << "saveAsFile " + currentFileName << oendl;
    if(currentFileName.isEmpty()) {
        return saveAs();
    }
    if(doc) {
        file = doc->file();
        odebug << "saver file "+file << oendl;
        name = doc->name();
        odebug << "File named "+name << oendl;
    } else {
        file = currentFileName;
        name = QFileInfo(currentFileName).baseName();
    }

    QString rt = editor->text();
    if( !rt.isEmpty() ) {
        if(name.isEmpty()) {
            saveAs();
        }
        else {
            currentFileName = name;
            odebug << "saveFile "+currentFileName << oendl;

            if(!fileIs) {
                // Save file permissions
                struct stat buf;
                mode_t mode;
                bool setmode = true;
                if(stat(file, &buf))
                    setmode = false;
                else
                    mode = buf.st_mode;

                doc->setName(name);
                FileManager fm;
                if ( !fm.saveFile( *doc, rt ) ) {
                    QMessageBox::message(tr("Text Edit"),tr("Save Failed"));
                    return false;
                }

                // Restore file permissions
                if(setmode)
                    chmod( file, mode );
            } 
            else {
                odebug << "regular save file" << oendl;
                QFile f(file);
                if( f.open(IO_WriteOnly)) {
                    QCString crt = rt.utf8();
                    f.writeBlock(crt,crt.length());
                } 
                else {
                    QMessageBox::message(tr("Text Edit"),tr("Write Failed"));
                    return false;
                }
            }
            resetEditStatus();
            if(caption().left(1)=="*")
                setCaption(caption().right(caption().length()-1));
        }
        return true;
    }
    return false;
}

/*!
  prompted save */
bool TextEdit::saveAs()
{
    odebug << "saveAsFile " + currentFileName << oendl;

    QString rt = editor->text();
    odebug << currentFileName << oendl;

    QString docname;
    if( currentFileName.isEmpty()
        || currentFileName == tr("Unnamed")
        || currentFileName == tr("Text Editor"))
    {
        odebug << "do silly TT filename thing" << oendl;
        QString pt = rt.simplifyWhiteSpace();
        int i = pt.find( ' ' );
        docname = pt;
        if ( i > 0 )
            docname = pt.left( i );

        while( docname.startsWith( "." ) )
            docname = docname.mid( 1 );
        docname.replace( QRegExp("/"), "_" );
        // Cut the length. Filenames longer than 40 are not helpful
        // and something goes wrong when they get too long.
        if ( docname.length() > 40 ) docname = docname.left(40);

        if ( docname.isEmpty() ) docname = tr("Unnamed");
    }

    QMap<QString, QStringList> map;
    map.insert(tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr("Text"), text );
    text << "*";
    map.insert(tr("All"), text );

    QFileInfo cuFi( docname);
    QString filee = cuFi.fileName();
    QString dire = cuFi.dirPath();
    if(dire==".")
        dire = QPEApplication::documentDir();

    QString str;
    if( !featureAutoSave) {
        str = OFileDialog::getSaveFileName( 2, dire, filee, map);
        if(!str.isEmpty() && QFile::exists(str)) {
            switch ( QMessageBox::warning(this,tr("Text Editor"),tr("<p>The selected file already exists. Overwrite it?</p>"),tr("Overwrite"),tr("Cancel"),0,1) ) {
            case 0: //Overwrite
                break;
            case 1: //Cancel
                return false;
                break;
            }
        }
    }
    else
        str = docname;

    if(!str.isEmpty()) {
        QString fileNm=str;

        odebug << "saving filename "+fileNm << oendl;
        QFileInfo fi(fileNm);
        currentFileName=fi.fileName();
        if(doc)
            delete doc;

        DocLnk nf;
        nf.setType("text/plain");
        nf.setFile( fileNm);
        doc = new DocLnk(nf);
        odebug << "Saving file as " << currentFileName << oendl;
        doc->setName( fi.baseName() );
        updateCaption();

        FileManager fm;
        if ( !fm.saveFile( *doc, rt ) ) {
            QMessageBox::message(tr("Text Edit"),tr("Save Failed"));
            return false;
        }

        if( filePerms ) {
            filePermissions *filePerm;
            filePerm = new filePermissions(this, tr("Permissions"),true, 0,
                                           (const QString &)fileNm);
            QPEApplication::execDialog( filePerm );

            delete  filePerm;
        }
        resetEditStatus();
        if(caption().left(1)=="*")
            setCaption(caption().right(caption().length()-1));
        checkEnableDelete();

        return true;
    }
    odebug << "returning false" << oendl;
    return false;
} //end saveAs

void TextEdit::clear()
{
    delete doc;
    doc = 0;
    editor->clear();
}

void TextEdit::updateCaption()
{
    if ( currentFileName.isEmpty() )
        setCaption( tr("Text Editor") );
    else {
        QString s = currentFileName;
        int sep = s.findRev( '/' );
        if ( sep > 0 )
            s = s.mid( sep+1 );
        setCaption( tr("%1 - Text Editor").arg( s ) );
    }
}

void TextEdit::setDocument(const QString& fileref)
{
    if(fileref != tr("Unnamed")) {
        if(!savePrompt())
            return;

        currentFileName=fileref;
        odebug << "setDocument" << oendl;
        QFileInfo fi(currentFileName);
        odebug << "basename:" + fi.baseName() + ": current filename " + currentFileName << oendl;
        if( (fi.baseName().left(1)).isEmpty() ) {
            openDotFile(currentFileName);
        }
        else {
            odebug << "setDoc open" << oendl;
            openFile(fileref);
        }
    }
    updateCaption();
}

void TextEdit::changeFont()
{
    QDialog *d = new QDialog ( this, "FontDialog", true );
    d-> setCaption ( tr( "Choose font" ));
    QBoxLayout *lay = new QVBoxLayout ( d );
    OFontSelector *ofs = new OFontSelector ( true, d );
    lay-> addWidget ( ofs );
    ofs-> setSelectedFont ( editor-> font ( ));

    if ( QPEApplication::execDialog( d ) == QDialog::Accepted )
        editor-> setFont ( ofs-> selectedFont ( ));
    delete d;
}

void TextEdit::fileDelete()
{
    if(doc && ( currentFileName != "" ) ) {
        switch ( QMessageBox::warning(this,tr("Text Editor"),
                                    tr("Are you sure you want<BR>to <B>delete</B> "
                                        "the current file\nfrom disk?<BR>This is "
                                        "<B>irreversible!</B>"),
                                    tr("Yes"),tr("No"),0,1,1) ) {
        case 0:
            doc->removeFiles();
            clear();
            setCaption( tr("Text Editor") );
            break;
        case 1:
            // exit
            break;
        };
    }
}

void TextEdit::editorChanged()
{
    if( !edited ) {
        setCaption( "*"+caption());
        edited=true;
    }
}

void TextEdit::receive(const QCString&msg, const QByteArray &)
{
    odebug << "QCop "+msg << oendl;
    if ( msg == "setDocument(QString)" ) {
//        odebug << "bugger all" << oendl;
    }
}

void TextEdit::doDesktop(bool b)
{
    openDesktop=b;
    Config cfg("TextEdit");
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "OpenDesktop", b);
}

void TextEdit::doFilePerms(bool b)
{
    filePerms=b;
    Config cfg("TextEdit");
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "FilePermissions", b);
}

void TextEdit::editPasteTimeDate()
{
#ifndef QT_NO_CLIPBOARD
    QClipboard *cb = QApplication::clipboard();
    QDateTime dt = QDateTime::currentDateTime();
    cb->setText( dt.toString());
    editor->paste();
#endif
}

bool TextEdit::savePrompt()
{
    if(edited) {
        switch( QMessageBox::information( 0, (tr("Textedit")),
                                            (tr("Textedit detected\n"
                                                "you have unsaved changes\n"
                                                "Go ahead and save?\n")),
                                            (tr("Save")), (tr("Don't Save")), (tr("&Cancel")), 2, 2 ) ) {
        case 0:  // Save
            return save();
            break;

        case 1:  // Don't Save
            return true;
            break;

        case 2:  // Cancel
            return false;
            break;
        };
    }

    return true;
}

void TextEdit::timerCrank()
{
    if(featureAutoSave && edited) {
        if(currentFileName.isEmpty()) {
            currentFileName = QDir::homeDirPath()+"/textedit.tmp";
            saveAs();
        }
        else {
//            odebug << "autosave" << oendl;
            save();
        }
        setTimer();
    }
}

void TextEdit::doTimer(bool b)
{
    Config cfg("TextEdit");
    cfg.setGroup ( "View" );
    cfg.writeEntry ( "autosave", b);
    featureAutoSave = b;
    nAutoSave->setOn(b);
    if(b) {
//          odebug << "doTimer true" << oendl;
        setTimer();
    }
//    else
//        odebug << "doTimer false" << oendl;
}

void TextEdit::setTimer()
{
    if(featureAutoSave) {
//        odebug << "setting autosave" << oendl;
        QTimer *timer = new QTimer(this );
        connect( timer, SIGNAL(timeout()), this, SLOT(timerCrank()) );
        timer->start( 300000, true); //5 minutes
    }
}

void TextEdit::gotoLine()
{
    if( editor->length() < 1)
        return;
    QWidget *d = QApplication::desktop();
    gotoEdit = new QLineEdit( 0, "Goto line");

    gotoEdit->move( (d->width()/2) - ( gotoEdit->width()/2) , (d->height()/2) - (gotoEdit->height()/2));
    gotoEdit->setFrame(true);
    gotoEdit->show();
    connect (gotoEdit,SIGNAL(returnPressed()), this, SLOT(doGoto()));
}

void TextEdit::doGoto()
{
    QString number = gotoEdit->text();
    gotoEdit->hide();

    delete gotoEdit;
    gotoEdit = 0;

    bool ok;
    int lineNumber = number.toInt(&ok, 10);
    if( editor->numLines() < lineNumber)
        QMessageBox::message(tr("Text Edit"),tr("Not enough lines"));
    else
        editor->setCursorPosition(lineNumber, 0, false);
}
