/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
// changes added by L. J. Potter Sun 02-17-2002 21:31:31

#include "textedit.h"
#include "fileBrowser.h"
#include "fileSaver.h"
#include "filePermissions.h"

#include "fontDialog.h"

#include <qpe/fontdatabase.h>
#include <qpe/global.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
//#include <qpe/finddialog.h>

#include <qstringlist.h>
#include <qaction.h>
#include <qcolordialog.h>
#include <qfileinfo.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qcheckbox.h>
#include <unistd.h>
#include <sys/stat.h>

#include <stdlib.h> //getenv
/* XPM */
static char * filesave_xpm[] = {
"16 16 78 1",
"   c None",
".  c #343434",
"+  c #A0A0A0",
"@  c #565656",
"#  c #9E9E9E",
"$  c #525252",
"%  c #929292",
"&  c #676767",
"*  c #848484",
"=  c #666666",
"-  c #D8D8D8",
";  c #FFFFFF",
">  c #DBDBDB",
",  c #636363",
"'  c #989898",
")  c #2D2D2D",
"!  c #909090",
"~  c #AEAEAE",
"{  c #EAEAEA",
"]  c #575757",
"^  c #585858",
"/  c #8A8A8A",
"(  c #828282",
"_  c #6F6F6F",
":  c #C9C9C9",
"<  c #050505",
"[  c #292929",
"}  c #777777",
"|  c #616161",
"1  c #3A3A3A",
"2  c #BEBEBE",
"3  c #2C2C2C",
"4  c #7C7C7C",
"5  c #F6F6F6",
"6  c #FCFCFC",
"7  c #6B6B6B",
"8  c #959595",
"9  c #4F4F4F",
"0  c #808080",
"a  c #767676",
"b  c #818181",
"c  c #B8B8B8",
"d  c #FBFBFB",
"e  c #F9F9F9",
"f  c #CCCCCC",
"g  c #030303",
"h  c #737373",
"i  c #7A7A7A",
"j  c #7E7E7E",
"k  c #6A6A6A",
"l  c #FAFAFA",
"m  c #505050",
"n  c #9D9D9D",
"o  c #333333",
"p  c #7B7B7B",
"q  c #787878",
"r  c #696969",
"s  c #494949",
"t  c #555555",
"u  c #949494",
"v  c #E6E6E6",
"w  c #424242",
"x  c #515151",
"y  c #535353",
"z  c #3E3E3E",
"A  c #D4D4D4",
"B  c #0C0C0C",
"C  c #353535",
"D  c #474747",
"E  c #ECECEC",
"F  c #919191",
"G  c #7D7D7D",
"H  c #000000",
"I  c #404040",
"J  c #858585",
"K  c #323232",
"L  c #D0D0D0",
"M  c #1C1C1C",
"    ...+        ",
"   @#$%&..+     ",
"   .*=-;;>,..+  ",
"  ')!~;;;;;;{]..",
"  ^/(-;;;;;;;_:<",
"  [}|;;;;;;;{12$",
" #34-55;;;;678$+",
" 90ab=c;dd;e1fg ",
" [ahij((kbl0mn$ ",
" op^q^^7r&]s/$+ ",
"@btu;vbwxy]zAB  ",
"CzDEvEv;;DssF$  ",
"G.H{E{E{IxsJ$+  ",
"  +...vEKxzLM   ",
"     +...z]n$   ",
"        +...    "};


#if QT_VERSION < 300

class QpeEditor : public QMultiLineEdit
{
    //    Q_OBJECT
public:
    QpeEditor( QWidget *parent, const char * name = 0 )
  : QMultiLineEdit( parent, name )
        {
            clearTableFlags();
            setTableFlags( Tbl_vScrollBar | Tbl_autoHScrollBar );
        }

    void find( const QString &txt, bool caseSensitive,
                            bool backwards );
//public slots:
      /*
signals:
    void notFound();
    void searchWrapped();
    */

private:

};


void QpeEditor::find ( const QString &txt, bool caseSensitive,
           bool backwards )
{
    static bool wrap = FALSE;
    int line, col;
    if ( wrap ) {
        if ( !backwards )
            line = col = 0;
        wrap = FALSE;
          //  emit searchWrapped();
    } else {
        getCursorPosition( &line, &col );
    }
      //ignore backwards for now....
    if ( !backwards ) {
        for ( ; ; ) {
            if ( line >= numLines() ) {
                wrap = TRUE;
                  //emit notFound();
                break;
            }
            int findCol = getString( line )->find( txt, col, caseSensitive );
            if ( findCol >= 0 ) {
                setCursorPosition( line, findCol, FALSE );
                col = findCol + txt.length();
                setCursorPosition( line, col, TRUE );

                  //found = TRUE;
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




static int u_id = 1;
static int get_unique_id()
{
    return u_id++;
}

static const int nfontsizes = 6;
static const int fontsize[nfontsizes] = {8,10,12,14,18,24};

TextEdit::TextEdit( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f ), bFromDocView( FALSE )
{
    doc = 0;
    edited=FALSE;
    edited1=FALSE;
    setToolBarsMovable( FALSE );

    setIcon( Resource::loadPixmap( "TextEditor" ) );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setHorizontalStretchable( TRUE );
    menu = bar;

    QPEMenuBar *mb = new QPEMenuBar( bar );
    QPopupMenu *file = new QPopupMenu( this );
    QPopupMenu *edit = new QPopupMenu( this );
    font = new QPopupMenu( this );

    bar = new QPEToolBar( this );
    editBar = bar;

    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
//    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Browse" ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( newFileOpen() ) );
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Save" ), QPixmap(( const char** ) filesave_xpm  ) , QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( save() ) );
//      a->addTo( bar );
    file->insertSeparator();
    a->addTo( file );

    a = new QAction( tr( "Save As" ), QPixmap(( const char** ) filesave_xpm  ) , QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( saveAs() ) );
    a->addTo( file );

    a = new QAction( tr( "Cut" ), Resource::loadPixmap( "cut" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Copy" ), Resource::loadPixmap( "copy" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Paste" ), Resource::loadPixmap( "paste" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( editBar );
    a->addTo( edit );

     a = new QAction( tr( "Find..." ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( editFind() ) );
     edit->insertSeparator();
     a->addTo( bar );
     a->addTo( edit );

     
    int defsize;
    bool defb, defi, wrap;

    Config cfg("TextEdit");
    cfg.setGroup("View");
    defsize = cfg.readNumEntry("FontSize",10);
    defb = cfg.readBoolEntry("Bold",FALSE);
    defi = cfg.readBoolEntry("Italic",FALSE);
    wrap = cfg.readBoolEntry("Wrap",TRUE);

    zin = new QAction( tr("Zoom in"), QString::null, 0, this, 0 );
    connect( zin, SIGNAL( activated() ), this, SLOT( zoomIn() ) );
    zin->addTo( font );

    zout = new QAction( tr("Zoom out"), QString::null, 0, this, 0 );
    connect( zout, SIGNAL( activated() ), this, SLOT( zoomOut() ) );
    zout->addTo( font );

    font->insertSeparator();

#if 0
    QAction *ba = new QAction( tr("Bold"), QString::null, 0, this, 0 );
    connect( ba, SIGNAL( toggled(bool) ), this, SLOT( setBold(bool) ) );
    ba->setToggleAction(TRUE);
    ba->addTo( font );

    QAction *ia = new QAction( tr("Italic"), QString::null, 0, this, 0 );
    connect( ia, SIGNAL( toggled(bool) ), this, SLOT( setItalic(bool) ) );
    ia->setToggleAction(TRUE);
    ia->addTo( font );

    ba->setOn(defb);
    ia->setOn(defi);

    font->insertSeparator();
#endif

    QAction *wa = new QAction( tr("Wrap lines"), QString::null, 0, this, 0 );
    connect( wa, SIGNAL( toggled(bool) ), this, SLOT( setWordWrap(bool) ) );
    wa->setToggleAction(TRUE);
    wa->addTo( font );

    font->insertSeparator();
    font->insertItem("Font", this, SLOT(changeFont()) );

    font->insertSeparator();
    nStart = new QAction( tr("Start with new file"), QString::null, 0, this, 0 );
    connect( nStart, SIGNAL( toggled(bool) ), this, SLOT( changeStartConfig(bool) ) );
    nStart->setToggleAction(TRUE);
    nStart->addTo( font );

    mb->insertItem( tr( "File" ), file );
    mb->insertItem( tr( "Edit" ), edit );
    mb->insertItem( tr( "View" ), font );

    searchBar = new QPEToolBar(this);
    addToolBar( searchBar,  "Search", QMainWindow::Top, TRUE );

    searchBar->setHorizontalStretchable( TRUE );

    searchEdit = new QLineEdit( searchBar, "searchEdit" );
    searchBar->setStretchableWidget( searchEdit );
    connect( searchEdit, SIGNAL( textChanged( const QString & ) ),
       this, SLOT( search() ) );


     a = new QAction( tr( "Find Next" ), Resource::loadPixmap( "next" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( findNext() ) );
     a->addTo( searchBar );
     a->addTo( edit );

    a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
    a->addTo( searchBar );

     edit->insertSeparator();
     a = new QAction( tr( "Delete" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( editDelete() ) );
     a->addTo( edit );
     
    searchBar->hide();

    editorStack = new QWidgetStack( this );
    setCentralWidget( editorStack );

    searchVisible = FALSE;

    fileSelector = new FileSelector( "text/*", editorStack, "fileselector" , TRUE, TRUE); //buggy
    connect( fileSelector, SIGNAL( closeMe() ), this, SLOT( showEditTools() ) );
    connect( fileSelector, SIGNAL( newSelected( const DocLnk &) ), this, SLOT( newFile( const DocLnk & ) ) );
    connect( fileSelector, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( openFile( const DocLnk & ) ) );
//      fileOpen();

    editor = new QpeEditor( editorStack );
    editor->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    editorStack->addWidget( editor, get_unique_id() );
    connect( editor, SIGNAL( textChanged() ), this, SLOT( editorChanged() ) );

    resize( 200, 300 );

//    setFontSize(defsize,TRUE);
    FontDatabase fdb;
    QFont defaultFont=editor->font();
    QFontInfo fontInfo(defaultFont);

    cfg.setGroup("Font");
    QString family = cfg.readEntry("Family", fontInfo.family());
    QString style = cfg.readEntry("Style", fdb.styleString(defaultFont));
    int i_size = cfg.readNumEntry("Size", fontInfo.pointSize()/10);
    QString charSet = cfg.readEntry("CharSet", QFont::encodingName( fontInfo.charSet()) );

    defaultFont = fdb.font(family,style,i_size,charSet);
    editor->setFont( defaultFont);

    wa->setOn(wrap);
    updateCaption();

    cfg.setGroup("View");
    if(cfg.readEntry("startNew","TRUE") == "TRUE") {
        nStart->setOn(TRUE);
        fileNew();
    } else {
        fileOpen();
    }

}

TextEdit::~TextEdit()
{
//    save();

    Config cfg("TextEdit");
    cfg.setGroup("View");
    QFont f = editor->font();
    cfg.writeEntry("FontSize",f.pointSize());
    cfg.writeEntry("Bold",f.bold());
    cfg.writeEntry("Italic",f.italic());
    cfg.writeEntry("Wrap",editor->wordWrap() == QMultiLineEdit::WidgetWidth);
}

void TextEdit::zoomIn()
{
    setFontSize(editor->font().pointSize()+1,FALSE);
}

void TextEdit::zoomOut()
{
    setFontSize(editor->font().pointSize()-1,TRUE);
}


void TextEdit::setFontSize(int sz, bool round_down_not_up)
{
    int s=10;
    for (int i=0; i<nfontsizes; i++) {
  if ( fontsize[i] == sz ) {
      s = sz;
      break;
  } else if ( round_down_not_up ) {
      if ( fontsize[i] < sz )
    s = fontsize[i];
  } else {
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

void TextEdit::setBold(bool y)
{
    QFont f = editor->font();
    f.setBold(y);
    editor->setFont(f);
}

void TextEdit::setItalic(bool y)
{
    QFont f = editor->font();
    f.setItalic(y);
    editor->setFont(f);
}

void TextEdit::setWordWrap(bool y)
{
    bool state = editor->edited();
    editor->setWordWrap(y ? QMultiLineEdit::WidgetWidth : QMultiLineEdit::NoWrap );
    editor->setEdited( state );
}

void TextEdit::fileNew()
{
    if( !bFromDocView  ) {
        saveAs();
    }
    newFile(DocLnk());
}

void TextEdit::fileOpen()
{
//      if ( !save() ) {
//    if ( QMessageBox::critical( this, tr( "Out of space" ),
//              tr( "Text Editor was unable to\n"
//            "save your changes.\n"
//            "Free some space and try again.\n"
//            "\nContinue anyway?" ),
//              QMessageBox::Yes|QMessageBox::Escape,
//              QMessageBox::No|QMessageBox::Default )
//         != QMessageBox::Yes )
//        return;
//    else {
//        delete doc;
//        doc = 0;
//    }
//      }
    menu->hide();
    editBar->hide();
    searchBar->hide();
    clearWState (WState_Reserved1 );
    editorStack->raiseWidget( fileSelector );
    fileSelector->reread();
    updateCaption();
}

void TextEdit::newFileOpen()
{
    browseForFiles=new fileBrowser(this,"Open File",TRUE,0, "*");
    if( browseForFiles->exec() != -1 ) {
        QString selFile= browseForFiles->selectedFileName;
        QStringList fileList=browseForFiles->fileList;
        qDebug(selFile);
        QStringList::ConstIterator f;
        QString fileTemp;
        for (  f = fileList.begin(); f != fileList.end(); f++ ) {
            fileTemp = *f;
            fileTemp.right( fileTemp.length()-5);
            QString  fileName = fileTemp;
            if( fileName != "Unnamed" || fileName != "Empty Text"  ) {
                currentFileName = fileName;
                qDebug("please open "+currentFileName);
                openFile(fileName );
            }
        }
    }
    delete browseForFiles;
    editor->setEdited( FALSE);
    edited1=FALSE;
    edited=FALSE;
    if(caption().left(1)=="*")
        setCaption(caption().right(caption().length()-1));
}

#if 0
void TextEdit::slotFind()
{
    FindDialog frmFind( "Text Editor", this );
    connect( &frmFind, SIGNAL(signalFindClicked(const QString &, bool, bool, int)),
       editor, SLOT(slotDoFind( const QString&,bool,bool)));

    //case sensitive, backwards, [category]

    connect( editor, SIGNAL(notFound()),
       &frmFind, SLOT(slotNotFound()) );
    connect( editor, SIGNAL(searchWrapped()),
       &frmFind, SLOT(slotWrapAround()) );

    frmFind.exec();


}
#endif

void TextEdit::fileRevert()
{
    clear();
    fileOpen();
}

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
    searchVisible = TRUE;
    searchEdit->setFocus();
}

void TextEdit::findNext()
{
    editor->find( searchEdit->text(), FALSE, FALSE );

}

void TextEdit::findClose()
{
    searchVisible = FALSE;
    searchBar->hide();
}

void TextEdit::search()
{
    editor->find( searchEdit->text(), FALSE, FALSE );
}

void TextEdit::newFile( const DocLnk &f )
{
    DocLnk nf = f;
    nf.setType("text/plain");
    clear();
    editorStack->raiseWidget( editor );
    setWState (WState_Reserved1 );
    editor->setFocus();
    doc = new DocLnk(nf);
    qDebug("newFile "+currentFileName);
    updateCaption(currentFileName);
}

void TextEdit::openFile( const QString &f )
{
    bFromDocView = TRUE;
    DocLnk nf;
    nf.setType("text/plain");
    nf.setFile(f);
    currentFileName=f;
    QFileInfo fi( currentFileName);
    nf.setName(fi.baseName());
    qDebug("openFile string"+currentFileName);

    openFile(nf);
    showEditTools();
    // Show filename in caption
    QString name = f;
    int sep = name.findRev( '/' );
    if ( sep > 0 )
  name = name.mid( sep+1 );
    updateCaption( name );
}

void TextEdit::openFile( const DocLnk &f )
{
//    clear();
    bFromDocView = TRUE;
    FileManager fm;
    QString txt;
    currentFileName=f.name();
    qDebug("openFile doclnk " + currentFileName);
    if ( !fm.loadFile( f, txt ) ) {
  // ####### could be a new file
        qDebug( "Cannot open file" );
 
  //return;
    }
    
    fileNew();
    if ( doc )
  delete doc;
    doc = new DocLnk(f);
    editor->setText(txt);
    editor->setEdited( FALSE);
    edited1=FALSE;
    edited=FALSE;

    qDebug("openFile doclnk "+currentFileName);
    doc->setName(currentFileName);
    updateCaption();
}

void TextEdit::showEditTools()
{
//        if ( !doc )
//      close();
//    clear();
    fileSelector->hide();
    menu->show();
    editBar->show();
    if ( searchVisible )
  searchBar->show();
//    updateCaption();
    editorStack->raiseWidget( editor );
    setWState (WState_Reserved1 );
}

/*!
  unprompted save */
bool TextEdit::save()
{
    QString file = doc->file();
    qDebug(file);
    QString name= doc->name();
    qDebug(name);
    QString rt = editor->text();
    if( !rt.isEmpty() ) {
        if(name.isEmpty()) {
            saveAs();
        } else {
            currentFileName= name ;
            qDebug("saveFile "+currentFileName);

            struct stat buf;
            mode_t mode;
            stat(file.latin1(), &buf);
            mode = buf.st_mode;

            doc->setName( name);
            FileManager fm;
            if ( !fm.saveFile( *doc, rt ) ) {
                return false;
            }
            editor->setEdited( FALSE);
            edited1=FALSE;
            edited=FALSE;
            setCaption(caption().right(caption().length()-1));


            chmod( file.latin1(), mode);
        }
        return true;
    }
    return false;
}

/*!
  prompted save */
bool TextEdit::saveAs()
{
//        qDebug("saveAsFile "+currentFileName);

      // case of nothing to save... /// there's always something to save
//          if ( !doc )//|| !bFromDocView)
//            {
//                qDebug("no doc");
//                return true;
//      }
    if ( !editor->edited() ) {
        delete doc;
        doc = 0;
        return true;
    }

    QString rt = editor->text();
    qDebug(currentFileName);
    
    if( currentFileName.isEmpty() || currentFileName == "Unnamed") {
        qDebug("do silly TT filename thing");
        if ( doc->name().isEmpty() ) {
            QString pt = rt.simplifyWhiteSpace();
            int i = pt.find( ' ' );
            QString docname = pt;
            if ( i > 0 )
                docname = pt.left( i );
              // remove "." at the beginning
            while( docname.startsWith( "." ) )
                docname = docname.mid( 1 );
            docname.replace( QRegExp("/"), "_" );
              // cut the length. filenames longer than that don't make sense and something goes wrong when they get too long.
            if ( docname.length() > 40 )
                docname = docname.left(40);
            if ( docname.isEmpty() )
                docname = "Unnamed";
            doc->setName(docname);
            currentFileName=docname;
        }
    }

    
    fileSaveDlg=new fileSaver(this,"Save File As?",TRUE, 0, currentFileName);
    qDebug("wanna save filename "+currentFileName);
    fileSaveDlg->exec();
    if( fileSaveDlg->result() == 1 ) {
        QString fileNm=fileSaveDlg->selectedFileName;
        qDebug("saving filename "+fileNm);
        QFileInfo fi(fileNm);
        currentFileName=fi.fileName();
        if(doc) {
//        QString file = doc->file();
//        doc->removeFiles();
            delete doc;
            DocLnk nf;
            nf.setType("text/plain");
            nf.setFile( fileNm);
            doc = new DocLnk(nf);
//        editor->setText(rt);
//            qDebug("openFile doclnk "+currentFileName);
            doc->setName( currentFileName);
            updateCaption( currentFileName);

            FileManager fm;
            if ( !fm.saveFile( *doc, rt ) ) {
                return false;
            }
            if( fileSaveDlg->filePermCheck->isChecked() ) {
                filePermissions *filePerm;
                filePerm = new filePermissions(this, "Permissions",true,0,(const QString &)fileNm);
                filePerm->exec();

                if( filePerm)
                    delete  filePerm;
            }
        }
    }
    editor->setEdited( FALSE);
    edited1=FALSE;
    edited=FALSE;
    if(caption().left(1)=="*")
    setCaption(caption().right(caption().length()-1));

    if(fileSaveDlg)
        delete fileSaveDlg;
    return true;
}

void TextEdit::clear()
{
    delete doc;
    doc = 0;
    editor->clear();
}

void TextEdit::updateCaption( const QString &name )
{
    if ( !doc )
  setCaption( tr("Text Editor") );
    else {
  QString s = name;
  if ( s.isNull() )
      s = doc->name();
  if ( s.isEmpty()  ) {
      s = tr( "Unnamed" );
       currentFileName=s;
  }

  setCaption( s + " - " + tr("Text Editor") );
    }
}

void TextEdit::setDocument(const QString& fileref)
{
    bFromDocView = TRUE;
    qDebug("setDocument "+fileref);
    bFromDocView = TRUE;
    if(fileref.find(".desktop",0,TRUE) == -1) {
    openFile(fileref);
    } else {
      openFile(DocLnk(fileref));
    }
}

void TextEdit::closeEvent( QCloseEvent *e )
{
    if ( editorStack->visibleWidget() == fileSelector && !bFromDocView ) {
        e->ignore();
        repaint();
//        fileRevert();
  
    } else {
        bFromDocView = FALSE;
        e->accept();
    }
}

void TextEdit::accept()
 {
    QString file = doc->file();
      if (file.find("_.txt",0,TRUE) ==-1)
         save();
      else {
          QFile(file).remove();
      }
    exit(0);

}

void TextEdit::changeFont() {
    FontDatabase fdb;
    QFont defaultFont=editor->font();
    QFontInfo fontInfo(defaultFont);
    Config cfg("TextEdit");
    cfg.setGroup("Font");
    QString family = cfg.readEntry("Family", fontInfo.family());
    QString style = cfg.readEntry("Style", fdb.styleString(defaultFont));
    int i_size = cfg.readNumEntry("Size", fontInfo.pointSize()/10);
    QString charSet = cfg.readEntry("CharSet", QFont::encodingName( fontInfo.charSet()) );

    defaultFont = fdb.font(family,style,i_size,charSet);

    FontDialog *fontDlg;
    fontDlg=new FontDialog(this,"FontDialog",TRUE);

    fontDlg->exec();

    QFont myFont=fontDlg->selectedFont;
    editor->setFont( myFont);
    delete fontDlg;

}

void TextEdit::editDelete()
{
    switch ( QMessageBox::warning(this,"Text Editor","Do you really want\nto delete the current file\nfrom the disk?\nThis is irreversable!!","Yes","No",0,0,1) ) {
      case 0:
          if(doc) {
              doc->removeFiles();
              clear();
              setCaption( tr("Text Editor") );
          }
          break;
      case 1: 
            // exit
          break;
    };
}

void TextEdit::changeStartConfig( bool b ) {

    Config cfg("TextEdit");
    cfg.setGroup("View");
    if(b) {
        qDebug("bool");
        cfg.writeEntry("startNew","TRUE");
    } else {
        cfg.writeEntry("startNew","FALSE");
    }
    update();
}

void TextEdit::editorChanged() { 
    if(editor->edited() && edited && !edited1) {
        setCaption( "*"+caption());
        edited1=TRUE;
    }
    edited=TRUE;
}
