/****************************************************************************
**
** Created: Sat Apr 6 17:57:45 2002
**
** Author: Carsten Schneider <CarstenSchneider@t-online.de>
**
** $Id: zsafe.cpp,v 1.6 2003-08-13 16:54:10 zcarsten Exp $
**
** Homepage: http://home.t-online.de/home/CarstenSchneider/zsafe/index.html
**
** Compile Flags:
**    Zaurus arm     : -DNO_OPIE
**    Zaurus Opie arm: none
**    Linux Desktop  : -DDESKTOP
**    Windows Desktop: -DDESKTOP -DWIN32
**
**    for japanese version additional use: -DJPATCH_HDE
**
****************************************************************************/
#include "zsafe.h"
#include "newdialog.h"
#include "searchdialog.h"
#include "categorydialog.h"
#include "passworddialog.h"
#include "infoform.h"
#include "zlistview.h"
#include "shadedlistitem.h"

#ifndef DESKTOP
#ifndef NO_OPIE
#include <opie/ofiledialog.h>
#else
#include "scqtfileedit.h"
#endif
#endif

#include <qclipboard.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#include <qmenubar.h>
#include <qpopupmenu.h>

#ifdef DESKTOP
#include <qfiledialog.h>
#ifndef WIN32
#include <qsettings.h>
#else
#include "qsettings.h"
#endif
#include <qapplication.h>
#else
#include <qfile.h>
#include <qpe/fileselector.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#endif

#include <qtimer.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qheader.h>
#include <qlistview.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qregexp.h>
#include <qdir.h>
#include <qtextbrowser.h>
#include <qlabel.h>
#include <qcombobox.h>

#include "krc2.h"

#include "wait.h"

extern int DeskW, DeskH;
#ifdef DESKTOP
extern QApplication   *appl;
#else
extern QPEApplication *appl;
#endif


#ifdef DESKTOP
#ifndef WIN32
const QString APP_KEY="/.zsafe/";
#else
const QString APP_KEY="";
#endif
#else
const QString APP_KEY="";
#endif

// include xmp images
#include "pics/zsafe/copy.xpm"
#include "pics/zsafe/cut.xpm"
#include "pics/zsafe/edit.xpm"
#include "pics/zsafe/editdelete.xpm"
#include "pics/zsafe/find.xpm"
#include "pics/zsafe/folder_open.xpm"
#include "pics/zsafe/help_icon.xpm"
#include "pics/zsafe/new.xpm"
#include "pics/zsafe/paste.xpm"
#include "pics/zsafe/quit_icon.xpm"
#include "pics/zsafe/save.xpm"
#include "pics/zsafe/trash.xpm"
#include "pics/zsafe/expand.xpm"
#include "pics/zsafe/export.xpm"
#include "pics/zsafe/import.xpm"
#include "pics/zsafe/zsafe.xpm"

static const char* const bank_cards_data[] = { 
"14 14 16 1",
". c None",
"# c #000000",
"b c #0000de",
"a c #0000e6",
"j c #41de83",
"k c #4acecd",
"h c #4aced5",
"g c #5a40cd",
"d c #5a44d5",
"l c #9440d5",
"m c #b4ce4a",
"n c #cd4883",
"e c #d5ae10",
"f c #de3ce6",
"i c #e640e6",
"c c #ffffff",
"..............",
".###########..",
".#ababababa#..",
".#babbbabbb#..",
".#ccccccccc#..",
".#cdcefcghc#..",
".#ccccccccc#..",
".#cicjkclic#..",
".#ccccccccc#..",
".#cmchlcnec#..",
".#ccccccccc#..",
".###########..",
"..............",
".............."};


static const char* const passwords_data[] = { 
"16 16 20 1",
". c None",
"# c #000000",
"r c #000083",
"p c #0000c5",
"q c #0000ff",
"n c #008100",
"l c #00c200",
"m c #00ff00",
"j c #838100",
"a c #c55900",
"h c #c5c200",
"o c #c5c2ff",
"k c #c5ffc5",
"f c #ff0000",
"d c #ff8100",
"b c #ffaa5a",
"e c #ffc2c5",
"c c #ffdeac",
"i c #ffff00",
"g c #ffffc5",
"............###.",
"...........#abb#",
"..........#cbab#",
".........#cbdd#.",
"######..#cbdd#..",
"#eeff#..#add#...",
"#eeff#######....",
"#ccdbdd#........",
"#dddbdd###......",
"#gghihhjj#......",
"#hhhihhjj###....",
"#kklmllnnnn#....",
"#lllmllnnnn#....",
"#oopqpprprr#....",
"#oopqpprprr#....",
"############...."};

static const char* const software_data[] = { 
"16 16 5 1",
". c None",
"# c #000000",
"b c #838183",
"c c #c5ffff",
"a c #ffffff",
"................",
".##############.",
"#aaaaaaaaaaaaaa#",
"#abbbbbbbbbbbbb#",
"#ab##########ab#",
"#ab#c########ab#",
"#ab#c#c######ab#",
"#ab##########ab#",
"#ab##########ab#",
"#ab##########ab#",
"#ab##########ab#",
"#ab##########ab#",
"#aaaaaaaaaaaaab#",
"#bbbbbbbbbbbbbb#",
".##############.",
"................"};

static const char* const general_data[] = { 
"14 14 98 2",
"Qt c None",
".k c #000000",
"#x c #080808",
"#F c #101008",
"#q c #101010",
"#i c #101410",
"## c #101810",
".m c #181818",
".3 c #181c18",
".I c #182018",
".T c #202420",
"#D c #202820",
"#y c #292c29",
".c c #293029",
".d c #313031",
"#E c #313429",
"#r c #313831",
".j c #393c31",
"#j c #394039",
"#C c #414841",
".w c #4a554a",
".a c #4a594a",
".# c #525052",
".l c #52594a",
"#f c #525952",
"#v c #525d52",
".O c #5a4c4a",
".9 c #5a595a",
".A c #5a5d52",
".B c #624c52",
".0 c #625552",
"#o c #626562",
".R c #626962",
"#. c #626d5a",
"#p c #626d62",
".2 c #627162",
"#h c #6a6d62",
"#z c #6a7562",
"#w c #6a756a",
".C c #73656a",
".P c #73696a",
"#a c #737d6a",
".U c #738573",
".E c #7b817b",
"#B c #7b857b",
"#s c #7b897b",
"#n c #7b917b",
".b c #838d83",
".7 c #839583",
".n c #8b7d7b",
"#g c #8b8583",
".g c #8b858b",
".r c #8b898b",
".s c #8b8d8b",
".i c #8b9183",
".8 c #8b918b",
"#A c #8b9d8b",
".S c #8ba183",
".Z c #94918b",
".N c #949994",
".F c #949d94",
".x c #94a18b",
".v c #94a194",
".Y c #94aa94",
".h c #9c999c",
".Q c #9ca19c",
"#u c #9ca59c",
".H c #9caa9c",
"#e c #9cb29c",
"#m c #a4b29c",
"#t c #a4b2a4",
".M c #a4b69c",
"#l c #a4b6a4",
".z c #a4baa4",
".f c #aca5ac",
".q c #acaaac",
"#d c #acbeac",
".6 c #acc2ac",
".o c #b4b2b4",
".t c #b4beb4",
"#k c #b4c2ac",
".5 c #b4cab4",
".D c #bdb6bd",
".G c #bdc6b4",
"#c c #bdceb4",
".X c #bdd2bd",
".4 c #bdd6bd",
".1 c #c5bec5",
".e c #c5c2c5",
".u c #c5cac5",
"#b c #c5d6c5",
".J c #c5dec5",
".p c #cdcacd",
".W c #cddecd",
".L c #cde2cd",
".K c #d5eacd",
".V c #d5ead5",
".y c #d5eed5",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQt.#.a.b.cQtQtQtQtQtQtQt",
"QtQt.d.e.f.g.h.i.c.j.dQt.kQt",
".a.l.m.n.o.p.q.r.s.t.u.v.wQt",
".x.y.z.A.B.C.D.p.q.E.F.G.H.I",
".I.J.K.L.M.N.O.P.o.p.Q.R.S.T",
"Qt.U.V.L.W.X.Y.Z.0.P.1.s.2.3",
"Qt.3.X.W.4.X.5.6.7.8.9.s#.##",
"QtQt#a.X#b#c.5.6#d#e#f#g#h#i",
"QtQtQt#j.7#k.6#d#l#m#n#o#p#q",
"QtQtQtQt.k#r#s#m#t.H#u#v#w#x",
"QtQtQtQtQtQt.k#y#z.v#A#B#C#x",
"QtQtQtQtQtQtQtQt.k#D.w#s#E.k",
"QtQtQtQtQtQtQtQtQtQtQt#x#FQt"};

// exit ZSafe and clear the clipboard for security reasons
  void ZSafe::exitZs (int ec)
  {
     QClipboard *cb = QApplication::clipboard();
     cb->clear();

     exit (ec);
  }


// save the configuration into the file
  void ZSafe::saveConf ()
  {
    if (conf)
    {
       delete conf;

#ifdef DESKTOP
#ifndef WIN32
       conf = new QSettings ();
       conf->insertSearchPath (QSettings::Unix, QDir::homeDirPath());
#else
       conf = new QSettings (cfgFile);
       conf->insertSearchPath (QSettings::Unix, cfgFile);
#endif
#else
       conf = new Config (cfgFile, Config::File);
       conf->setGroup ("zsafe");
#endif
    }
  }


/* 
 *  Constructs a ZSafe which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ZSafe::ZSafe( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    IsCut = false;
    IsCopy = false;
    modified = false;

    // set the config file
    cfgFile=QDir::homeDirPath();
    cfgFile += "/.zsafe.cfg";
    // set the icon path
    QString qpedir ((const char *)getenv("QPEDIR"));
#ifdef DESKTOP
    iconPath = QDir::homeDirPath() + "/pics/";
#else
    if (qpedir.isEmpty())
       iconPath = "/home/QtPalmtop/pics/";
    else 
       iconPath = qpedir + "/pics/";
#endif

    // create a zsafe configuration object
#ifdef DESKTOP
#ifndef WIN32
    conf = new QSettings ();
    conf->insertSearchPath (QSettings::Unix, QDir::homeDirPath());
#else
    conf = new QSettings (cfgFile);
    conf->insertSearchPath (QSettings::Unix, cfgFile);
#endif
#else
    conf = new Config (cfgFile, Config::File);
    conf->setGroup ("zsafePrefs");
#endif
#ifdef DESKTOP
// #ifndef WIN32
    expandTree = conf->readBoolEntry(APP_KEY+"expandTree", false);
// #endif
#else
    expandTree = conf->readNumEntry(APP_KEY+"expandTree", 0);
#endif
#ifndef DESKTOP
    conf->setGroup ("zsafe");
#endif

    QPixmap copy_img((const char**) copy_xpm);
    QPixmap cut_img((const char**) cut_xpm);
    QPixmap edit_img((const char**) edit_xpm);
    QPixmap editdelete_img((const char**) editdelete_xpm);
    QPixmap find_img((const char**) find_xpm);
    QPixmap folder_open_img((const char**) folder_open_xpm);
    QPixmap help_icon_img((const char**) help_icon_xpm);
    QPixmap new_img((const char**) new_xpm);
    QPixmap paste_img((const char**) paste_xpm);
    QPixmap quit_icon_img((const char**) quit_icon_xpm);
    QPixmap save_img((const char**) save_xpm);
    QPixmap trash_img((const char**) trash_xpm);
    QPixmap expand_img((const char**) expand_xpm);
    QPixmap export_img((const char**) export_xpm);
    QPixmap import_img((const char**) import_xpm);

    QPixmap bank_cards( ( const char** ) bank_cards_data );
    QPixmap passwords( ( const char** ) passwords_data );
    QPixmap software( ( const char** ) software_data );
    QPixmap general( ( const char** ) general_data );
    if ( !name )
	setName( "ZSafe" );

#ifdef DESKTOP
#ifdef WIN32
    setGeometry(100, 150, DeskW, DeskH-30 ); 
#else
    resize( DeskW, DeskH-30 ); 
#endif

#else

#ifdef JPATCH_HDE
   int DeskS;
   if(DeskW > DeskH)
   {
      DeskS = DeskW;
   }
   else
   {
      DeskS = DeskH;
   }
   resize( DeskW, DeskH );
   setMinimumSize( QSize( DeskS, DeskS ) );
   setMaximumSize( QSize( DeskS, DeskS ) );
#else
    resize( DeskW, DeskH-30 );
#endif

#endif
    // setCaption( tr( "ZSafe" ) );

    filename = conf->readEntry(APP_KEY+"document");
   if (filename.isEmpty() || filename.isNull())
   {

    // check if the directory application exists, if not
    // create it
// #ifndef WIN32
    // QString d1("Documents/application");
// #else
	QString d1(QDir::homeDirPath() + "/Documents/application");
// #endif
    QDir pd1(d1);
    if (!pd1.exists())
    {
       QDir pd1("Documents");
       if (!pd1.mkdir("application", FALSE))
       {
          QMessageBox::critical( 0, tr("ZSafe"),
               tr("Can't create directory\n%1\n\nZSafe will now exit.").arg(d1));
          exitZs (1);
       }
    }
// #ifndef WIN32
    // QString d2("Documents/application/zsafe");
// #else
	QString d2(QDir::homeDirPath() + "/Documents/application/zsafe");
// #endif
    QDir pd2(d2);
    if (!pd2.exists())
    {
       QDir pd2("Documents/application");
       if (!pd2.mkdir("zsafe", FALSE))
       {
          QMessageBox::critical( 0, tr("ZSafe"),
               tr("Can't create directory\n%1\n\nZSafe will now exit.").arg(d2));
          exitZs (1);
       }
    }
    

    // set the default filename
    filename=d2 + "/passwords.zsf";

    // save the current filename to the config file
    conf->writeEntry(APP_KEY+"document", filename);
    saveConf();
   }

   //if (filename == "INVALIDPWD")
      //filename = "";

    QString ti = filename.right (filename.length() - filename.findRev ('/') - 1);
#ifdef WIN32
    this->setCaption("Qt ZSafe: " + ti);
#else
    this->setCaption("ZSafe: " + ti);
#endif

    selectedItem = NULL;
    lastSearchedCategory = NULL;
    lastSearchedItem = NULL;
    lastSearchedName = "";
    lastSearchedUsername = "";
    lastSearchedComment = "";

    infoForm = new InfoForm();
    categoryDialog = NULL;

    // add a menu bar
    QMenuBar *menu = new QMenuBar( this );

    // add file menu
    // QPopupMenu *file = new QPopupMenu( this );
    file = new QPopupMenu( this );

// #ifdef DESKTOP
    file->insertItem( new_img, tr("&New document"),  this, SLOT(newDocument()) );
    file->insertItem( folder_open_img, tr("&Open document"),  this, SLOT(loadDocument()) );
    file->insertItem( save_img, tr("&Save document as .."),  this, SLOT(saveDocumentAs()) );
    file->insertSeparator();
// #endif

    file->insertItem( save_img, tr("&Save document"),  this, SLOT(saveDocumentWithoutPwd()) );
    file->insertItem( save_img, tr("S&ave document with new Password"),  this, 
                      SLOT(saveDocumentWithPwd()) );
    file->insertSeparator();
    file->insertItem( export_img, tr("&Export text file"),  this, SLOT(writeAllEntries()) );
    file->insertItem( import_img, tr("&Import text file"),  this, SLOT(readAllEntries()) );
    file->insertItem( trash_img, tr("&Remove text file"),  this, SLOT(removeAsciiFile()) );
    file->insertSeparator();
    file->insertItem( expand_img, tr("&Open entries expanded"), this, 
                      SLOT(setExpandFlag()), 0, 'o');
    file->setItemChecked('o', expandTree);
    file->insertSeparator();
    file->insertItem( quit_icon_img, tr("E&xit"),  this, SLOT(quitMe()) );
    menu->insertItem( tr("&File"), file );

    QPopupMenu *cat = new QPopupMenu( this );
    cat->insertItem( new_img, tr("&New"),  this, SLOT(addCategory()) );
    cat->insertItem( edit_img, tr("&Edit"),  this, SLOT(editCategory()) );
    cat->insertItem( trash_img, tr("&Delete"),  this, SLOT(delCategory()) );
    menu->insertItem( tr("&Category"), cat );

    QPopupMenu *it = new QPopupMenu( this );
    it->insertItem( cut_img, tr("&Cut"),  this, SLOT(cutItem()) );
    it->insertItem( copy_img, tr("C&opy"),  this, SLOT(copyItem()) );
    it->insertItem( paste_img, tr("&Paste"),  this, SLOT(pasteItem()) );
    it->insertSeparator();
    it->insertItem( new_img, tr("&New"),  this, SLOT(newPwd()) );
    it->insertItem( edit_img, tr("&Edit"),  this, SLOT(editPwd()) );
    it->insertItem( trash_img, tr("&Delete"),  this, SLOT(deletePwd()) );
    it->insertItem( find_img, tr("&Search"),  this, SLOT(findPwd()) );
    menu->insertItem( tr("&Entry"), it );

    QPopupMenu *help = new QPopupMenu( this );
    help->insertItem( help_icon_img, tr("&About"),  this, SLOT(about()) );
    menu->insertItem( tr("&Help"), help );

    // toolbar icons

    New = new QToolButton( menu, "New" );
    New->setGeometry( QRect( DeskW-84, 2, 20, 20 ) ); 
    New->setMouseTracking( TRUE );
    New->setText( tr( "" ) );
    New->setPixmap( new_img );
    QToolTip::add(  New, tr( "New entry" ) );

    Edit = new QToolButton( menu, "Edit" );
    Edit->setGeometry( QRect( DeskW-64, 2, 20, 20 ) ); 
    Edit->setText( tr( "" ) );
    Edit->setPixmap( edit_img );
    QToolTip::add(  Edit, tr( "Edit category or entry" ) );

    Delete = new QToolButton( menu, "Delete" );
    Delete->setGeometry( QRect( DeskW-44, 2, 20, 20 ) ); 
    Delete->setText( tr( "" ) );
    Delete->setPixmap( trash_img );
    QToolTip::add(  Delete, tr( "Delete category or entry" ) );

    Find = new QToolButton( menu, "Find" );
    Find->setGeometry( QRect( DeskW-24, 2, 20, 20 ) ); 
    Find->setText( tr( "" ) );
    Find->setPixmap( find_img );
    QToolTip::add(  Find, tr( "Find entry" ) );

/*
    QBoxLayout * h = new QHBoxLayout( this );
    h->addWidget (menu);
    h->addWidget (New);
    h->addWidget (Edit);
    h->addWidget (Delete);
    h->addWidget (Find);
*/

    ListView = new ZListView( this, "ListView" );
    ListView->addColumn( tr( "Name" ) );
    ListView->addColumn( tr( "Field 2" ) );
    ListView->addColumn( tr( "Field 3" ) );
    ListView->addColumn( tr( "Comment" ) );
    ListView->addColumn( tr( "Field 4" ) );
    ListView->addColumn( tr( "Field 5" ) );
    ListView->setAllColumnsShowFocus(TRUE);

#ifdef DESKTOP
    ListView->setResizePolicy(QScrollView::AutoOneFit);
    // ListView->setGeometry( QRect( 0, 22, this->width(), this->height() - 30 ) ); 
#else
    ListView->setResizePolicy(QScrollView::AutoOneFit);
    // ListView->setGeometry( QRect( 0, 22, 
                           // this->width(), this->height() - 30 ) ); 
    // ListView->setMaximumSize( QSize( 440, 290 ) );
#endif
    ListView->setVScrollBarMode( QListView::Auto );

    QBoxLayout * l = new QVBoxLayout( this );
    l->addWidget (menu);
    l->addWidget (ListView);

#ifndef DESKTOP
    // start a timer (100 ms) to load the default document
    docuTimer.start( 100, true );
    connect( &docuTimer, SIGNAL(timeout()), SLOT( slotLoadDocu() ) );
    raiseFlag = true;
    connect( &raiseTimer, SIGNAL(timeout()), SLOT( slotRaiseTimer() ) );
#else
    // open the default document
    openDocument(filename);
#endif

    // signals and slots connections for QTollButton
    connect( New, SIGNAL( clicked() ), this, SLOT( newPwd() ) );
    connect( Edit, SIGNAL( clicked() ), this, SLOT( editPwd() ) );
    connect( Delete, SIGNAL( clicked() ), this, SLOT( deletePwd() ) );
    connect( Find, SIGNAL( clicked() ), this, SLOT( findPwd() ) );
    // signals and slots connections for QListView
    connect( ListView, SIGNAL( selectionChanged( QListViewItem* ) ),
             this, SLOT( listViewSelected( QListViewItem* ) ) );
    connect( ListView, SIGNAL( doubleClicked( QListViewItem* ) ),
             this, SLOT( showInfo( QListViewItem* ) ) );
    connect( ListView, SIGNAL( returnPressed( QListViewItem* ) ),
             this, SLOT( showInfo( QListViewItem* ) ) );

}

const QColor *ZSafe::evenRowColor = &Qt::white;
// const QColor *ZSafe::oddRowColor = &Qt::lightGray;
const QColor *ZSafe::oddRowColor = new QColor(216,240,255);

/*  
 *  Destroys the object and frees any allocated resources
 */
ZSafe::~ZSafe()
{
    // no need to delete child widgets, Qt does it all for us
    quitMe();
}

// load the default document
void ZSafe::slotLoadDocu()
{
   openDocument (filename);
}

void ZSafe::deletePwd()
{

    if (!selectedItem)
       return;
    if (!isCategory(selectedItem))
    {
       switch( QMessageBox::information( this, tr("ZSafe"),
                                      tr("Do you want to delete?"),
                                      tr("&Delete"), tr("D&on't Delete"), 
                                      0      // Enter == button 0
                                      ) ) { // Escape == button 2
       case 0: // Delete clicked, Alt-S or Enter pressed.
          // Delete
          modified = true;
          selectedItem->parent()->takeItem(selectedItem);
          selectedItem = NULL;
          break;
       case 1: // Don't delete
          break;
       }
    }
    else
    {
       delCategory();
    }
}

void ZSafe::editPwd()
{
    if (!selectedItem)
       return;
    if (!isCategory(selectedItem))
    {
       // open the 'New Entry' dialog
       NewDialog *dialog = new NewDialog(this, tr("Edit Entry"), TRUE);
#ifdef WIN32
       dialog->setCaption ("Qt " + tr("Edit Entry"));
       dialog->setGeometry(200, 250, 220, 310 ); 
#endif
    
       // set the labels
       dialog->Name->setText(getFieldLabel (selectedItem,     "1", tr("Name")));
       dialog->Username->setText(getFieldLabel (selectedItem, "2", tr("Username")));
       dialog->Password->setText(getFieldLabel (selectedItem, "3", tr("Password")));
       dialog->Comment->setText(getFieldLabel (selectedItem,  "4", tr("Comment")));
       dialog->Field5Label->setText(getFieldLabel (selectedItem,"5", tr("Field 4")));
       dialog->Field6Label->setText(getFieldLabel (selectedItem,"6", tr("Field 5")));

       // set the fields
       dialog->NameField->setText(selectedItem->text (0));
       dialog->UsernameField->setText(selectedItem->text (1));
       dialog->PasswordField->setText(selectedItem->text (2));
       QString comment = selectedItem->text (3);
       comment.replace (QRegExp("<br>"), "\n");
       dialog->Field5->setText(selectedItem->text (4));
       dialog->Field6->setText(selectedItem->text (5));
       dialog->CommentField->insertLine(comment);
       dialog->CommentField->setCursorPosition(0,0);
#ifdef DESKTOP
#ifndef WIN32
       dialog->show();
#endif
#else
       dialog->showMaximized();
#endif
       DialogCode result = (DialogCode) dialog->exec();

#ifdef DESKTOP
       result = Accepted;
#endif
       if (result == Accepted)
       {
#ifdef JPATCH_HDE
          // edit the selected item
          QString name = dialog->NameField->text();
          selectedItem->setText (0, name);
          QString user = dialog->UsernameField->text();
          selectedItem->setText (1, user);
          QString pwd = dialog->PasswordField->text();
          selectedItem->setText (2, pwd);
          QString comment = dialog->CommentField->text();
          comment.replace (QRegExp("\n"), "<br>");
          selectedItem->setText (3, comment);
          QString f5 = dialog->Field5->text();
          selectedItem->setText (4, f5);
          QString f6 = dialog->Field6->text();
          selectedItem->setText (5, f6);
#else
          modified = true;
          // edit the selected item
          QString name = dialog->NameField->text(); 
          selectedItem->setText (0, tr (name));
          QString user = dialog->UsernameField->text(); 
          selectedItem->setText (1, tr (user));
          QString pwd = dialog->PasswordField->text(); 
          selectedItem->setText (2, tr (pwd));
          QString comment = dialog->CommentField->text(); 
          comment.replace (QRegExp("\n"), "<br>");
          selectedItem->setText (3, tr (comment));
          QString f5 = dialog->Field5->text(); 
          selectedItem->setText (4, tr (f5));
          QString f6 = dialog->Field6->text(); 
          selectedItem->setText (5, tr (f6));
#endif
       }

       delete dialog;
    }
    else 
    {
       editCategory();
    }
}

void ZSafe::newPwd()
{
    if (!selectedItem)
       return;

    if (!isCategory(selectedItem))
         selectedItem = selectedItem->parent();

    if (isCategory(selectedItem))
    {
       QString cat = selectedItem->text(0);

       // open the 'New Entry' dialog
       NewDialog *dialog = new NewDialog(this, tr("New Entry"), TRUE);
#ifdef WIN32
       dialog->setCaption ("Qt " + tr("New Entry"));
       dialog->setGeometry(200, 250, 220, 310 ); 
#endif
       // set the labels
       dialog->Name->setText(getFieldLabel (selectedItem,     "1", tr("Name")));
       dialog->Username->setText(getFieldLabel (selectedItem, "2", tr("Username")));
       dialog->Password->setText(getFieldLabel (selectedItem, "3", tr("Password")));
       dialog->Comment->setText(getFieldLabel (selectedItem,  "4", tr("Comment")));
       dialog->Field5Label->setText(getFieldLabel (selectedItem,"5", tr("Field 4")));
       dialog->Field6Label->setText(getFieldLabel (selectedItem,"6", tr("Field 5")));
retype:
#ifdef DESKTOP
#ifndef WIN32
       dialog->show();
#endif
#else
       dialog->showMaximized();
#endif
       DialogCode result = (DialogCode) dialog->exec();
#ifdef DESKTOP
       result = Accepted;
#endif

       if (result == Accepted)
       {
        
          QString name = dialog->NameField->text(); 
          if (cat == name)
          {
             QMessageBox::critical( 0, tr("ZSafe"),
                tr("Entry name must be different\nfrom the category name.") );
             goto retype; // it's not a good programming style :-)
          }

          modified = true;
          // add the new item
          QListViewItem *i = new ShadedListItem (0, selectedItem);
          i->setOpen (TRUE);

#ifdef JPATCH_HDE
          i->setText (0, name);
          QString user = dialog->UsernameField->text();
          i->setText (1, user);
          QString pwd = dialog->PasswordField->text();
          i->setText (2, pwd);
          QString comment = dialog->CommentField->text();
          comment.replace (QRegExp("\n"), "<br>");
          i->setText (3, comment);
          QString f5 = dialog->Field5->text();
          i->setText (4, f5);
          QString f6 = dialog->Field6->text();
          i->setText (5, f6);
#else
          i->setText (0, tr (name));
          QString user = dialog->UsernameField->text(); 
          i->setText (1, tr (user));
          QString pwd = dialog->PasswordField->text(); 
          i->setText (2, tr (pwd));
          QString comment = dialog->CommentField->text(); 
          comment.replace (QRegExp("\n"), "<br>");
          i->setText (3, tr (comment));
          QString f5 = dialog->Field5->text(); 
          i->setText (4, tr (f5));
          QString f6 = dialog->Field6->text(); 
          i->setText (5, tr (f6));
#endif
       }

       delete dialog;
    }
}

void ZSafe::findPwd()
{

    // open the 'Search' dialog
    SearchDialog *dialog = new SearchDialog(this, tr("Search"), TRUE);
#ifdef WIN32
    dialog->setCaption ("Qt " + tr("Search"));
#endif

#ifdef DESKTOP
#endif
    if (lastSearchedName)
       dialog->NameField->setText(lastSearchedName);
    else
       dialog->NameField->setText("");
    if (lastSearchedUsername)
       dialog->UsernameField->setText(lastSearchedUsername);
    else
       dialog->UsernameField->setText("");
    if (lastSearchedComment)
       dialog->CommentField->setText(lastSearchedComment);
    else
       dialog->CommentField->setText("");
    DialogCode result = (DialogCode) dialog->exec();
#ifdef DESKTOP
       result = Accepted;
#endif

    QString name;
    QString username;
    QString comment;
    if (result == Accepted)
    {
       name = dialog->NameField->text();
       username = dialog->UsernameField->text();
       comment = dialog->CommentField->text();
       qWarning (name);
    }
    else
    {
       delete dialog;
       return;
    }

    if (!name.isEmpty() && name != lastSearchedName ||
        lastSearchedName.isEmpty() && !name.isEmpty())
    {
       // set search at the beginning if a new name is given
       lastSearchedCategory = NULL;
       lastSearchedItem = NULL;
    }
    lastSearchedName = name;
    if (!username.isEmpty() && username != lastSearchedUsername ||
        lastSearchedUsername.isEmpty() && !username.isEmpty())
    {
       // set search at the beginning if a new name is given
       lastSearchedCategory = NULL;
       lastSearchedItem = NULL;
    }
    lastSearchedUsername = username;
    if (!comment.isEmpty() && comment != lastSearchedComment ||
        lastSearchedComment.isEmpty() && !comment.isEmpty())
    {
       // set search at the beginning if a new name is given
       lastSearchedCategory = NULL;
       lastSearchedItem = NULL;
    }
    lastSearchedComment = comment;

    ListView->clearSelection();

    bool found=FALSE;
    // step through all categories
    QListViewItem *i;
    if (lastSearchedCategory)
       i = lastSearchedCategory;
     else
       i = ListView->firstChild();
    for (; 
         i != NULL;
         i = i->nextSibling())
    {
       qWarning (i->text(0));
       i->setSelected(FALSE);

       // step through all subitems
       QListViewItem *si;
       if (lastSearchedItem)
          si = lastSearchedItem;
       else
          si = i->firstChild();
       // for (si = i->firstChild(); 
       for (;
            si != NULL;
            si = si->nextSibling())
       {
          qWarning (si->text(0));
          if (si->isSelected())
             si->setSelected(FALSE);
          // ListView->repaintItem(si);

          bool n=TRUE;
          bool u=TRUE;
          bool c=TRUE;
          if (!name.isEmpty())
             n = (si->text(0)).contains (name, FALSE);
          if (!username.isEmpty())
             u = (si->text(1)).contains (username, FALSE);
          if (!comment.isEmpty())
             c = (si->text(3)).contains (comment, FALSE);

          if ((n && u && c ) && !found)
          {
             qWarning ("Found");
             selectedItem = si;
             si->setSelected(TRUE);
             ListView->setCurrentItem(si);
             ListView->ensureItemVisible(si);
             ListView->triggerUpdate();

             lastSearchedCategory = i;
             // set to the next item
             lastSearchedItem = si->nextSibling();
             if (!lastSearchedItem)
             {
                // no next item within category -> set next category
                lastSearchedCategory = i->nextSibling();
                if (!lastSearchedCategory)
                   lastSearchedItem = NULL; // END
             }

             found = TRUE;
             delete dialog;
             update();
             return;
          }
       }
       lastSearchedCategory = i->nextSibling();
       lastSearchedItem = NULL;
    }
    lastSearchedCategory = NULL;
    lastSearchedItem = NULL;
    delete dialog;
    update();
    QMessageBox::information( this, tr("ZSafe"), 
                             tr("Entry not found"), tr("&OK"), 0);
        
}

QString ZSafe::getFieldLabel (QListViewItem *_item, QString field, QString def)
{
   QString category;
   if (_item)
   {
      if (isCategory(_item))
      {
         category = _item->text(0);
      }
      else
      {
         QListViewItem *cat = _item->parent();
         category =  cat->text(0);
      }
   }
   else
   {
      return def;
   }

   QString app_key = APP_KEY;
#ifndef DESKTOP
#ifndef WIN32
   conf->setGroup ("fieldDefs");
#endif
#else
#ifndef WIN32
   app_key += "/fieldDefs/";
#endif
#endif
// #ifndef WIN32
   QString label = conf->readEntry(app_key+category+"-field"+field,def);
// #else
//   QString label(def);
// #endif

#ifndef DESKTOP
   conf->setGroup ("zsafe");
#endif
   return label;
}

QString ZSafe::getFieldLabel (QString category, QString field, QString def)
{
   QString app_key = APP_KEY;
#ifndef DESKTOP
   conf->setGroup ("fieldDefs");
#else
#ifndef WIN32
   app_key += "/fieldDefs/";
#endif
#endif
// #ifndef WIN32
   QString label = conf->readEntry(app_key+category+"-field"+field, 
                                   def);
// #else
   // QString label(def);
// #endif
#ifndef DESKTOP
   conf->setGroup ("zsafe");
#endif
   return label;
}

void ZSafe::showInfo( QListViewItem *_item)
{
   if (!_item)
      return;
   if (selectedItem != NULL)
      selectedItem->setSelected(FALSE);
   
   selectedItem = _item;
   selectedItem->setSelected(TRUE);

   if (!isCategory(_item))
   {
/*
      QString label=selectedItem->text(0);
      label+="\n";
      label+=selectedItem->text(1);
      label+="\n";
      label+=selectedItem->text(2);
      label+="\n";
      label+=selectedItem->text(3);
*/

      QString text;
      QString entry;

      text = "<html><body><div align=""center""><u><b>";
      text += selectedItem->text(0);
      text += "</b></u><br></div><br>";

      entry = selectedItem->text(1);
      if (!entry.isEmpty() && entry.compare(" "))
      {
         text += "<u><b>";
         text += getFieldLabel (selectedItem, "2", tr("Username"));
         text += ":<br></b></u><blockquote>";
         text += entry;
         text += "</blockquote>";
         // text += "<br>";
      }

      entry = selectedItem->text(2);
      if (!entry.isEmpty() && entry.compare(" "))
      {
         text += "<u><b>";
         text += getFieldLabel (selectedItem, "3", tr("Password"));
         text += ":<br> </b></u><blockquote>";
         text += entry;
         text += "</blockquote>";
         // text += "<br>";
      }

      entry = selectedItem->text(4);
      if (!entry.isEmpty() && entry.compare(" "))
      {
         text += "<u><b>";
         text += getFieldLabel (selectedItem, "5", tr("Field 4"));
         text += ":<br> </b></u><blockquote>";
         text += entry;
         text += "</blockquote>";
         // text += "<br>";
      }

      entry = selectedItem->text(5);
      if (!entry.isEmpty() && entry.compare(" "))
      {
         text += "<u><b>";
         text += getFieldLabel (selectedItem, "6", tr("Field 5"));
         text += ":<br> </b></u><blockquote>";
         text += entry;
         text += "</blockquote>";
         // text += "<br>";
      }

      entry = selectedItem->text(3);
      if (!entry.isEmpty() && entry.compare(" "))
      {
         text += "<u><b>";
         text += getFieldLabel (selectedItem, "4", tr("Comment"));
         text += ":<br> </b></u>";
         QString comment = selectedItem->text(3);
         comment.replace (QRegExp("\n"), "<br>");
         text += comment;
         // text += "<br>";
      }

      text += "</body></html>";

      infoForm->InfoText->setText(text);
      infoForm->hide();
#ifdef DESKTOP
      infoForm->show();
#else
      infoForm->showMaximized();
#endif

   }
}

void ZSafe::listViewSelected( QListViewItem *_item)
{
   if (!_item)
      return;
   if (selectedItem != NULL)
      selectedItem->setSelected(FALSE);
   
   selectedItem = _item;

#ifndef DESKTOP
   // set the column text dependent on the selected item
   ListView->setColumnText(0, getFieldLabel (selectedItem, "1", tr("Name")));
   ListView->setColumnText(1, getFieldLabel (selectedItem, "2", tr("Field 2")));
   ListView->setColumnText(2, getFieldLabel (selectedItem, "3", tr("Field 3")));
   ListView->setColumnText(3, getFieldLabel (selectedItem, "4", tr("Comment")));
   ListView->setColumnText(4, getFieldLabel (selectedItem, "5", tr("Field 4")));
   ListView->setColumnText(5, getFieldLabel (selectedItem, "6", tr("Field 5")));
#endif
#ifdef WIN32
   // set the column text dependent on the selected item
   ListView->setColumnText(0, getFieldLabel (selectedItem, "1", tr("Name")));
   ListView->setColumnText(1, getFieldLabel (selectedItem, "2", tr("Field 2")));
   ListView->setColumnText(2, getFieldLabel (selectedItem, "3", tr("Field 3")));
   ListView->setColumnText(3, getFieldLabel (selectedItem, "4", tr("Comment")));
   ListView->setColumnText(4, getFieldLabel (selectedItem, "5", tr("Field 4")));
   ListView->setColumnText(5, getFieldLabel (selectedItem, "6", tr("Field 5")));
#endif

}

bool ZSafe::isCategory(QListViewItem *_item)
{
   if (_item == NULL)
      return FALSE;

   QString categoryName = _item->text (0);
   if (categories.find (categoryName))
      return TRUE;
   else
      return FALSE;
}

void ZSafe::removeAsciiFile()
{
    // QString fn = filename + ".txt";
  // open the file dialog
#ifndef DESKTOP
#ifndef NO_OPIE
   QMap<QString, QStringList> mimeTypes;
   mimeTypes.insert(tr("All"), QStringList() );
   mimeTypes.insert(tr("Text"), "text/*" );
   QString fn = OFileDialog::getOpenFileName( OFileSelector::EXTENDED_ALL,
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       QString::null,
                       mimeTypes,
                       this,
                       tr ("Remove text file"));
#else
  QString fn = ScQtFileEdit::getOpenFileName(this, 
                       tr ("Remove text file"),
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       "*.txt");
#endif
#else
  QString fn = QFileDialog::getOpenFileName(
	            QDir::homeDirPath() + "/Documents/application/zsafe",
                    "ZSafe (*.txt)",
                    this,
                    "ZSafe File Dialog"
                    "Choose a text file" );
#endif

  if (fn && fn.length() > 0 )
  {
    QFile f( fn );
    if ( !f.remove() ) 
    {
        qWarning( QString("Could not remove file %1").arg(fn),
                      2000 );
        QMessageBox::critical( 0, tr("ZSafe"),
                  tr("Could not remove text file.") );
        return;
    }
  }
}

void ZSafe::writeAllEntries()
{
   if (filename.isEmpty())
   {
       QMessageBox::critical( 0, tr("ZSafe"),
         tr("No document defined.\nYou have to create a new document"));
       return;
   }

   // open the file dialog
#ifndef DESKTOP
#ifndef NO_OPIE
   QMap<QString, QStringList> mimeTypes;
   mimeTypes.insert(tr("All"), QStringList() );
   mimeTypes.insert(tr("Text"), "text/*" );
   QString fn = OFileDialog::getSaveFileName( OFileSelector::EXTENDED_ALL,
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       QString::null,
                       mimeTypes,
                       this,
                       tr ("Export text file"));
#else
   QString fn = ScQtFileEdit::getSaveAsFileName(this, 
                       tr ("Export text file"),
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       "*.txt");
#endif
#else
   QString fn = QFileDialog::getSaveFileName(
	            QDir::homeDirPath() + "/Documents/application/zsafe",
                    "ZSafe (*.txt)",
                    this,
                    "ZSafe File Dialog"
                    "Choose a text file" );
#endif

   // open the new document
   if (fn && fn.length() > 0 )
   {
    QFile f( fn );
    if ( !f.open( IO_WriteOnly ) ) {
        qWarning( QString("Could not write to file %1").arg(fn),
                      2000 );
        QMessageBox::critical( 0, "ZSafe",
                  QString("Could not export to text file.") );
        return;
    }
    QTextStream t( &f );

    QListViewItem *i;
    // step through all categories
    for (i = ListView->firstChild(); 
         i != NULL;
         i = i->nextSibling())
    {
       // step through all subitems
       QListViewItem *si;
       for (si = i->firstChild(); 
            si != NULL;
            si = si->nextSibling())
       {
          QString oneEntry;
          oneEntry += "\"";
          oneEntry += i->text(0);
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(0);
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(1);
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(2);
          oneEntry += "\";";
          oneEntry += "\"";
          QString comment = si->text(3);
          comment.replace (QRegExp("\n"), "<br>");
          oneEntry += comment;
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(4);
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(5);
          oneEntry += "\"";
          // qWarning (oneEntry);
          t << oneEntry << endl;

          // qWarning (si->text(0));
       }
    }
    f.close();
   }
}

void ZSafe::readAllEntries()
{
   if (filename.isEmpty())
   {
       QMessageBox::critical( 0, tr("ZSafe"),
         tr("No document defined.\nYou have to create a new document"));
       return;
   }

  // open the file dialog
#ifndef DESKTOP
#ifndef NO_OPIE
   QMap<QString, QStringList> mimeTypes;
   mimeTypes.insert(tr("All"), QStringList() );
   mimeTypes.insert(tr("Text"), "text/*" );
   QString fn = OFileDialog::getOpenFileName( OFileSelector::EXTENDED_ALL,
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       QString::null,
                       mimeTypes,
                       this,
                       tr ("Import text file"));
#else
  QString fn = ScQtFileEdit::getOpenFileName(this, 
                       tr ("Import text file"),
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       "*.txt");
#endif
#else
  QString fn = QFileDialog::getOpenFileName(
	            QDir::homeDirPath() + "/Documents/application/zsafe",
                    "ZSafe (*.txt)",
                    this,
                    "ZSafe File Dialog"
                    "Choose a text file" );
#endif

  if (fn && fn.length() > 0 )
  {
   QFile f( fn );
   if ( !f.open( IO_ReadOnly ) )
   {
      qWarning( QString("Could not read file %1").arg(fn),
                      2000 );
      QMessageBox::critical( 0, "ZSafe",
                  QString("Could not import text file.") );
      return;
   }

   modified = true;

   // clear the password list
   selectedItem = NULL;
   QListViewItem *i;
   // step through all categories
   for (i = ListView->firstChild();
        i != NULL;
        i = i->nextSibling())
   {
      // step through all subitems
      QListViewItem *si;
      for (si = i->firstChild();
           si != NULL; )
           // si = si->nextSibling())
      {
          QListViewItem *_si = si;
          si = si->nextSibling();
          i->takeItem(_si); // remove from view list
          if (_si) delete _si;
      }
   }

   qWarning ("ReadAllEntries(): ");

   QTextStream t(&f);
   while ( !t.eof() ) 
   {
      QString s = t.readLine();
      s.replace (QRegExp("\";\""), "\"|\"");
      // char buffer[1024];
#ifndef WIN32
	  char buffer[s.length()+1];
#else
	  char buffer[4048];
#endif


      /* modify QString -> QCString::utf8 */
      
      strcpy (buffer, s.utf8());

      QString name;
      QString user;
      QString password;
      QString comment;
      QString field5="";
      QString field6="";

      // separete the entries
      char *i = strtok (buffer, "|");
      QString category(QString::fromUtf8(&i[1]));
      category.truncate(category.length() -1);

      int idx=0;
      while ((i = strtok (NULL, "|")) != NULL)
      {
         switch (idx)
         {
            case 0:
               name = QString::fromUtf8(&i[1]);
               name.truncate(name.length() -1);
               // name
               break;
            case 1:
               // user
               user = QString::fromUtf8(&i[1]);
               user.truncate(user.length() -1);
               break;
            case 2:
               // password
               password = QString::fromUtf8(&i[1]);
               password.truncate(password.length() -1);
               break;
            case 3:
               // comment
               comment = QString::fromUtf8(&i[1]);
               comment.truncate(comment.length() -1);
               break;
            case 4:
               // field5
               field5 = QString::fromUtf8(&i[1]);
               field5.truncate(field5.length() -1);
               break;
            case 5:
               // field6
               field6 = QString::fromUtf8(&i[1]);
               field6.truncate(field6.length() -1);
               break;
         }
         idx++; 
      }

      Category *cat= categories.find (category);
      if (cat)
      {
         // use the existend item
         QListViewItem *catItem = cat->getListItem();
         if (catItem)
         {
            QListViewItem * item = new ShadedListItem( 0, catItem );
#ifdef JPATCH_HDE
            item->setText( 0, name );
            item->setText( 1, user );
            item->setText( 2, password );
            item->setText( 3, comment );
            item->setText( 4, field5 );
            item->setText( 5, field6 );
#else
            item->setText( 0, tr( name ) );
            item->setText( 1, tr( user ) );
            item->setText( 2, tr( password ) );
            item->setText( 3, tr( comment ) );
            item->setText( 4, tr( field5 ) );
            item->setText( 5, tr( field6 ) );
            catItem->setOpen( TRUE );
#endif
         }
      }
      else
      {
         QListViewItem *catI = new ShadedListItem( 1, ListView );
         // create and insert a new item
         QListViewItem * item = new ShadedListItem( 0, catI );
#ifdef JPATCH_HDE
         item->setText( 0, name );
         item->setText( 1, user );
         item->setText( 2, password );
         item->setText( 3, comment );
         item->setText( 4, field5 );
         item->setText( 5, field6 );
#else
         item->setText( 0, tr( name ) );
         item->setText( 1, tr( user ) );
         item->setText( 2, tr( password ) );
         item->setText( 3, tr( comment ) );
         item->setText( 4, tr( field5 ) );
         item->setText( 5, tr( field6 ) );
#endif
         catI->setOpen( TRUE );

         Category *c1 = new Category();
         c1->setCategoryName(category);

         QString icon;
         QString fullIconPath;
         QPixmap *pix;
// #ifndef WIN32
         icon = conf->readEntry(APP_KEY+category);
// #endif
         bool isIconAv = false;
         if (!icon.isEmpty() && !icon.isNull())
         {
            // build the full path
            fullIconPath = iconPath + icon;
            pix = new QPixmap (fullIconPath);
            if (pix)
            {
               QImage img = pix->convertToImage();
               pix->convertFromImage(img.smoothScale(14,14));
               c1->setIconName (icon);
               c1->setIcon (*pix);
               isIconAv = true;
            }
         }
         if (!isIconAv)
         {
            c1->setIcon (*getPredefinedIcon(category));
         }
         c1->setListItem (catI);
         c1->initListItem();
         categories.insert (c1->getCategoryName(), c1);
      }

   }
   f.close();

  }
  else
  {
  }

}

#ifdef UNUSED
void ZSafe::writeAllEntries()
{
    // open the file for writing
    QString fn = filename + ".txt";
    QFile f( fn );
    if ( !f.open( IO_WriteOnly ) ) {
        qWarning( QString("Could not write to file %1").arg(fn),
                      2000 );
        QMessageBox::critical( 0, tr("ZSafe"),
                  tr("Could not export to text file.") );
        return;
    }
    QTextStream t( &f );

    QListViewItem *i;
    // step through all categories
    for (i = ListView->firstChild(); 
         i != NULL;
         i = i->nextSibling())
    {
       // step through all subitems
       QListViewItem *si;
       for (si = i->firstChild(); 
            si != NULL;
            si = si->nextSibling())
       {
          QString oneEntry;
          oneEntry += "\"";
          oneEntry += i->text(0);
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(0);
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(1);
          oneEntry += "\";";
          oneEntry += "\"";
          oneEntry += si->text(2);
          oneEntry += "\";";
          oneEntry += "\"";
          // oneEntry += si->text(3);
          QString comment = si->text(3);
          comment.replace (QRegExp("\n"), "<br>");
          oneEntry += comment;
          oneEntry += "\"";
          // qWarning (oneEntry);
          t << oneEntry << endl;

          // qWarning (si->text(0));
       }
    }
    f.close();
}

void ZSafe::readAllEntries()
{
   QString fn = filename + ".txt";
   QFile f( fn );
   if ( !f.open( IO_ReadOnly ) )
   {
      qWarning( QString("Could not read file %1").arg(fn),
                      2000 );
      QMessageBox::critical( 0, tr("ZSafe"),
                  tr("Could not import text file.") );
      return;
   }

   modified = true;

   // clear the password list
   selectedItem = NULL;
   QListViewItem *i;
   // step through all categories
   for (i = ListView->firstChild();
        i != NULL;
        i = i->nextSibling())
   {
      // step through all subitems
      QListViewItem *si;
      for (si = i->firstChild();
           si != NULL; )
           // si = si->nextSibling())
      {
          QListViewItem *_si = si;
          si = si->nextSibling();
          i->takeItem(_si); // remove from view list
          if (_si) delete _si;
      }
   }

   qWarning ("ReadAllEntries(): ");

   QTextStream t(&f);
   while ( !t.eof() ) 
   {
      QString s = t.readLine();
      s.replace (QRegExp("\";\""), "\"|\"");
      // char buffer[1024];
	  int len=s.length()+1;
#ifdef WIN32
	  char buffer[512];
#else
      char buffer[len];
#endif
      strcpy (buffer, s);

      QString name;
      QString user;
      QString password;
      QString comment;

      // separete the entries
      char *i = strtok (buffer, "|");
      QString category(&i[1]);
      category.truncate(category.length() -1);

      int idx=0;
      while (i = strtok (NULL, "|"))
      {
         switch (idx)
         {
            case 0:
               name = &i[1];
               name.truncate(name.length() -1);
               // name
               break;
            case 1:
               // user
               user = &i[1];
               user.truncate(user.length() -1);
               break;
            case 2:
               // password
               password = &i[1];
               password.truncate(password.length() -1);
               break;
            case 3:
               // comment
               comment = &i[1];
               comment.truncate(comment.length() -1);
               break;
         }
         idx++; 
      }

      Category *cat= categories.find (category);
      if (cat)
      {
         // use the existend item
         QListViewItem *catItem = cat->getListItem();
         if (catItem)
         {
            QListViewItem * item = new ShadedListItem( 0, catItem );
#ifdef JPATCH_HDE
            item->setText( 0, name );
            item->setText( 1, user );
            item->setText( 2, password );
            item->setText( 3, comment );
#else
            item->setText( 0, tr( name ) );
            item->setText( 1, tr( user ) );
            item->setText( 2, tr( password ) );
            item->setText( 3, tr( comment ) );
#endif
            catItem->setOpen( TRUE );
         }
      }
      else
      {
         QListViewItem *catI = new ShadedListItem( 1, ListView );
         // create and insert a new item
         QListViewItem * item = new ShadedListItem( 0, catI );
#ifdef JPATCH_HDE
         item->setText( 0, name );
         item->setText( 1, user );
         item->setText( 2, password );
         item->setText( 3, comment );
#else
         item->setText( 0, tr( name ) );
         item->setText( 1, tr( user ) );
         item->setText( 2, tr( password ) );
         item->setText( 3, tr( comment ) );
#endif
         catI->setOpen( TRUE );

         Category *c1 = new Category();
         c1->setCategoryName(category);

         QString icon;
         QString fullIconPath;
         QPixmap *pix;
// #ifndef WIN32
         icon = conf->readEntry(APP_KEY+category);
// #endif
         bool isIconAv = false;
         if (!icon.isEmpty() && !icon.isNull())
         {
            // build the full path
            fullIconPath = iconPath + icon;
            pix = new QPixmap (fullIconPath);
            if (pix)
            {
               QImage img = pix->convertToImage();
               pix->convertFromImage(img.smoothScale(14,14));
               c1->setIconName (icon);
               c1->setIcon (*pix);
               isIconAv = true;
            }
         }
         if (!isIconAv)
         {
            c1->setIcon (*getPredefinedIcon(category));
         }
         c1->setListItem (catI);
         c1->initListItem();
         categories.insert (c1->getCategoryName(), c1);
      }

   }
   f.close();

}
#endif // UNUSED

void ZSafe::resume(int)
{
   qWarning ("Resume");
   // hide the main window

   infoForm->hide();
   // open zsafe again
   m_password = "";
   selectedItem = NULL;

   // clear the password list
   QListViewItem *i;
   // step through all categories
   for (i = ListView->firstChild();
        i != NULL;
        i = i->nextSibling())
   {
      // step through all subitems
      QListViewItem *si;
      for (si = i->firstChild();
           si != NULL; )
      {
          QListViewItem *_si = si;
          si = si->nextSibling();
          i->takeItem(_si); // remove from view list
          if (_si) delete _si;
      }
   }

   // ask for password and read again
   openDocument(filename);
}

//---------------------------------------------


bool ZSafe::openDocument(const char* _filename, const char* )
{
     int retval;
     char* entry[FIELD_SIZE];
// #ifndef WIN32
     int validationFlag = conf->readNumEntry(APP_KEY+"valzsafe", 1);
// #else
	 // int validationFlag = 1;
// #endif

     int pwdOk = 0;
     int numberOfTries = 3;
     for (int i=0; i < numberOfTries; i++)
     {
        QFile f(_filename);
        if (f.exists())
        {
           // ask with a dialog for the password
	   if (m_password.isEmpty())
	      getDocPassword(tr("Enter Password"));
	   if (m_password.isEmpty() && validationFlag == 0)
           {
              qWarning ("Wrong password");
              QMessageBox::critical( 0, tr("ZSafe"),
                  tr("Wrong password.\n\nZSafe will now exit.") );
              exitZs (1);
           }

	   retval = loadInit(_filename, m_password);
	   if (retval != PWERR_GOOD) 
           {
                qWarning ("Error loading Document");
		return false;
	   }
        }
        else
        {
#ifdef WIN32
           this->setCaption("Qt ZSafe");
#else
           this->setCaption("ZSafe");
#endif
           filename = "";
           return false;
        }


        // load the validation entry
        if (validationFlag == 0)
        {
            pwdOk = 1;
            break;
        }

	retval = loadEntry(entry);
        if (retval == 1 &&
            !strcmp (entry[0], "ZSAFECATEGORY") &&
            !strcmp (entry[1], "name") &&
            !strcmp (entry[2], "username") &&
            !strcmp (entry[3], "password") &&
            !strcmp (entry[4], "comment") )
        {
	    for (int count = 0; count < FIELD_SIZE; count++) free(entry[count]);
            pwdOk = 1;
            break;
        }
        else
	   // for (int count = 0; count < FIELD_SIZE; count++) free(entry[count]);
	fclose (fd);
        m_password = "";

        if (i < numberOfTries - 1)
        {
           switch( QMessageBox::warning( this, tr("ZSafe"),
                    tr("Wrong password.\nEnter again?"),
                    tr("&Yes"), tr("&No."), 
                    0      
                    ) ) 
           { 
           case 1: // No
                 exitZs (1);
                 break;
           case 0:  // Yes
                 continue;
           }
        }
     }
     if (pwdOk == 0)
     {
        // unset the document entry
        conf->writeEntry(APP_KEY+"document", "INVALIDPWD");
        if (conf)
           delete conf;

        exitZs (1);
     }


	retval = loadEntry(entry);
        
        int numberOfEntries=0;
	while (retval == 1) {

                QString category( QString::fromUtf8(entry[0]) );
                QString name( QString::fromUtf8(entry[1]) );
                QString user( QString::fromUtf8(entry[2]) );
                QString password( QString::fromUtf8(entry[3]) );
                QString comment( QString::fromUtf8(entry[4]) );
                QString field5( QString::fromUtf8(entry[5]) );
                QString field6( QString::fromUtf8(entry[6]) );
                // add the subitems to the categories

                Category *cat= categories.find (category);
                if (cat)
                {
                   // use the existend item
                   QListViewItem *catItem = cat->getListItem();
                   if (catItem)
                   {
                      QListViewItem * item = new ShadedListItem( 0, catItem );
#ifdef JPATCH_HDE
                      item->setText( 0, name );
                      item->setText( 1, user );
                      item->setText( 2, password );
                      item->setText( 3, comment );
                      item->setText( 4, field5 );
                      item->setText( 5, field6 );
#else
                      item->setText( 0, tr( name ) );
                      item->setText( 1, tr( user ) );
                      item->setText( 2, tr( password ) );
                      item->setText( 3, tr( comment ) );
                      item->setText( 4, tr( field5 ) );
                      item->setText( 5, tr( field6 ) );
#endif
                      if (expandTree)
                         catItem->setOpen( TRUE );
                      numberOfEntries++;
                   }
                }
                else
                {
                   QListViewItem *catI = new ShadedListItem( 1, ListView );
                   // create and insert a new item
                   QListViewItem * item = new ShadedListItem( 0, catI );
#ifdef JPATCH_HDE
                   item->setText( 0, name );
                   item->setText( 1, user );
                   item->setText( 2, password );
                   item->setText( 3, comment );
                   item->setText( 4, field5 );
                   item->setText( 5, field6 );
#else
                   item->setText( 0, tr( name ) );
                   item->setText( 1, tr( user ) );
                   item->setText( 2, tr( password ) );
                   item->setText( 3, tr( comment ) );
                   item->setText( 4, tr( field5 ) );
                   item->setText( 5, tr( field6 ) );
#endif
                   if (expandTree)
                      catI->setOpen( TRUE );

                   Category *c1 = new Category();
                   c1->setCategoryName(category);

                   QString icon;
                   QString fullIconPath;
                   QPixmap *pix;
// #ifndef WIN32
                   icon = conf->readEntry(APP_KEY+category);
// #endif
                   bool isIconAv = false;
                   if (!icon.isEmpty() && !icon.isNull())
                   {
                      // build the full path
                      fullIconPath = iconPath + icon;
                      pix = new QPixmap (fullIconPath);
                      if (pix)
                      {
                         QImage img = pix->convertToImage();
                         pix->convertFromImage(img.smoothScale(14,14));
                         c1->setIconName (icon);
                         c1->setIcon (*pix);
                         isIconAv = true;
                      }
                   }
                   if (!isIconAv)
                   {
                      c1->setIcon (*getPredefinedIcon(category));
                   }

                   c1->setListItem (catI);
                   c1->initListItem();
                   categories.insert (c1->getCategoryName(), c1);
                   numberOfEntries++;
                }

		for (int count = 0; count < FIELD_SIZE; count++) {
			free(entry[count]);
		}
		retval = loadEntry(entry);
		if (retval == 2) {
			// m_parent->slotStatusHelpMsg("Last entry loaded");
		}
	} // end while

        if (numberOfEntries == 0)
        {
    
           switch( QMessageBox::warning( this, tr("ZSafe"),
                 tr("Empty document or\nwrong password.\nContinue?"),
                 tr("&No"), tr("&Yes."), 
                 0      
                 ) ) { 
           case 0: // No
	      retval = loadFinalize();
              exitZs (1);
              break;
           case 1:  // Yes
              break;
           }
	}

	retval = loadFinalize();

	return true;
}

int ZSafe::loadInit(const char* _filename, const char *password)
{
	unsigned int j = 0;
	unsigned int keylength=0;
	int count=0, count2=0, count3=0;
	unsigned char charbuf[8];
	unsigned short ciphertext[4];
	char key[128];
	Krc2* krc2 = new Krc2();

	fd = fopen (_filename, "rb");

        QFileInfo f (_filename);
        load_buffer_length = f.size();
        load_buffer_length = ((load_buffer_length / 1024)+1) * 1024 * 2;

	if (fd == NULL)
		return PWERR_OPEN;

	buffer = (char *)malloc(load_buffer_length);
	for (j = 0; password[j] != '\0'; j++) {
		key[j] = password[j];
	}
	keylength = j;
	krc2->rc2_expandkey (key, keylength, 128);

#ifndef WIN32
	size = read(fileno (fd), (unsigned char *) (charbuf + count), 8);
#else
	printf ("LoadInit() read1");
    size = fread ((unsigned char *) (charbuf + count), sizeof(unsigned char), 8, fd);
#endif

	if (size < 8)
		return PWERR_DATA;

	for (count = 0; count < 4; count++) {
		count2 = count << 1;
		iv[count] = charbuf[count2] << 8;
		iv[count] += charbuf[count2 + 1];
	}

	size = 0;
	bufferIndex = 0;
#ifndef WIN32
	while ((count = read (fileno (fd), (unsigned char *) charbuf, 8)) > 0) {
		while (count < 8) {
			count2 = read (fileno (fd), (unsigned char *) (charbuf + count), 8);
#else
	printf ("LoadInit() read2");
    while ((count = fread ((unsigned char *) (charbuf), sizeof(unsigned char), 8, fd)) > 0) {
		while (count < 8) {
			count2 = fread ((unsigned char *) (charbuf + count), sizeof(unsigned char), 8, fd);
#endif
			if (count2 == 0) {
				return PWERR_DATA;
			}
			count += count2;
		} /* while (count < 8) */

		size += 8;
		for (count2 = 0; count2 < 8; count2 += 2) {
			count3 = count2 >> 1;
			ciphertext[count3] = charbuf[count2] << 8;
			ciphertext[count3] += charbuf[count2 + 1];

			plaintext[count3] = ciphertext[count3] ^ iv[count3];
			iv[count3] = plaintext[count3];
		} /* for (count2) */

		krc2->rc2_decrypt (plaintext);
		memcpy ((unsigned char *) (buffer + bufferIndex), plaintext, 8);
		bufferIndex += 8;
		buffer[bufferIndex + 1] = '\0';
	} /* while ((count = read (fileno (fd), (unsigned char *) charbuf, 8)) > 0) */
	size -= buffer[size - 1];
	lastcount = 0;

	/* This will point to the starting index */
	bufferIndex = 0;
	return PWERR_GOOD;
}

int ZSafe::loadEntry(char *entry[FIELD_SIZE])
{
	/* Strip off PKCS 5 padding
	 * Should check to make sure it's good here
	 */
	int count, count1=0;

	for (count = lastcount; count < size; count++) {
		if ((unsigned char) (buffer[count]) == 255) {
			if (buffer[bufferIndex] == '\0') {
				bufferIndex++;
			}
			entry[count1] = (char *) malloc (count - bufferIndex + 1);
			memcpy (entry[count1], (unsigned char *) (buffer + bufferIndex), count - bufferIndex);
			entry[count1][count - bufferIndex] = '\0';
			count++;
			bufferIndex = count;
			count1++;
			if (count1 == FIELD_SIZE) {
				lastcount = count;
				return 1;
			}
		} /* if ((unsigned char) (buffer[count]) == 255) */
	} /* for (count = 0; count < size; count++) */

	return 2;
}

int ZSafe::loadFinalize(void)
{
	fclose (fd);
        if (buffer) free(buffer);
	return PWERR_GOOD;
}

bool ZSafe::saveDocument(const char* _filename, 
                         bool withPwd, 
                         const char* )
{
    if (filename.isEmpty())
    {
       QMessageBox::critical( 0, tr("ZSafe"),
         tr("No document defined.\nYou have to create a new document"));
       return false;
    }

     // if (m_password.isEmpty())
        // withPwd = true; // the document must be saved with a valid password
     if (withPwd)
     {
        bool pwdOk = FALSE;
        while (!pwdOk)
        {
	   getDocPassword(tr("Enter Password"));
	   if (m_password.isEmpty())
           {
              
              QMessageBox::critical( 0, tr("ZSafe"), 
                      tr("Password is empty.\nPlease enter again."));
              continue;
           }
        	
           QString firstPasswd = m_password;
   
	   getDocPassword(tr("Reenter Password"));
	   if (m_password.isEmpty())
           {
              QMessageBox::critical( 0, tr("ZSafe"), 
                      tr("Password is empty.\nPlease enter again."));
              continue;
           }
           if (firstPasswd != m_password)
           {
              
              QMessageBox::critical( 0, tr("ZSafe"), 
                      tr("Passwords must be identical.\nPlease enter again."));
              continue;
           }
           pwdOk = TRUE; 
           modified = false;
        }
     } 
     else if (modified)
     {
      QString fns(_filename);
      fns = fns.right (fns.length() - fns.findRev ('/') - 1);
      switch( QMessageBox::information( this, tr("ZSafe"),
          tr("Do you want to save ") + fns + tr("\nbefore continuing?"),
          tr("&Save"), 
          tr("&Don't Save"), 
          0      // Enter == button 0
          ) ) 
      { // Escape == button 2
      case 0: // Save clicked, Alt-S or Enter pressed.
        modified = false;
        break;
      case 1: // Don't Save clicked or Alt-D pressed
        modified = false;
        return true;
      }
     }
     modified = false;
	
     if (m_password.isEmpty())
        return false;

     int retval = saveInit(_filename, m_password);
	// int retval = saveInit(_filename, "test");
	if (retval != PWERR_GOOD) {
		return false;
     }
	
	char* entry[FIELD_SIZE];

        // save the validation entry
        {
           int i=0;
	   entry[i] = (char*)malloc(strlen("ZSAFECATEGORY")+1);
	   strcpy(entry[i++], "ZSAFECATEGORY");
	   entry[i] = (char*)malloc(strlen("name")+1);
	   strcpy(entry[i++], "name");
	   entry[i] = (char*)malloc(strlen("username")+1);
	   strcpy(entry[i++], "username");
	   entry[i] = (char*)malloc(strlen("password")+1);
	   strcpy(entry[i++], "password");
	   entry[i] = (char*)malloc(strlen("comment")+1);
	   strcpy(entry[i++], "comment");

	   entry[i] = (char*)malloc(strlen("field5")+1);
	   strcpy(entry[i++], "field5");
	   entry[i] = (char*)malloc(strlen("field6")+1);
	   strcpy(entry[i++], "field6");

	   retval = saveEntry(entry);
           for (int z=0; z<i; z++) free(entry[z]);
           if (retval == PWERR_DATA) {
              qWarning("1: Error writing file, contents not saved");
              saveFinalize();
              return false;
           }
// #ifndef WIN32
           conf->writeEntry(APP_KEY+"valzsafe", 1);
// #endif
           saveConf();
        }

        QListViewItem *i;
        // step through all categories
        for (i = ListView->firstChild();
             i != NULL;
             i = i->nextSibling())
        {
           // step through all subitems
           QListViewItem *si;
           for (si = i->firstChild();
                si != NULL;
                si = si->nextSibling())
           {
              int j=0;
         entry[j] = (char*)malloc(strlen(i->text(0).utf8())+1);
         strcpy(entry[j++], i->text(0).utf8());
         entry[j] = (char*)malloc(strlen(si->text(0).utf8())+1);
         strcpy(entry[j++], si->text(0).utf8());
         entry[j] = (char*)malloc(strlen(si->text(1).utf8())+1);
         strcpy(entry[j++], si->text(1).utf8());
         entry[j] = (char*)malloc(strlen(si->text(2).utf8())+1);
         strcpy(entry[j++], si->text(2).utf8());
         entry[j] = (char*)malloc(strlen(si->text(3).utf8())+1);
         strcpy(entry[j++], si->text(3).utf8());
         entry[j] = (char*)malloc(strlen(si->text(4).utf8())+1);
         strcpy(entry[j++], si->text(4).utf8());
         entry[j] = (char*)malloc(strlen(si->text(5).utf8())+1);
         strcpy(entry[j++], si->text(5).utf8());

	      retval = saveEntry(entry);
              for (int z=0; z<j; z++)
              {
                  free(entry[z]);
              }
              if (retval == PWERR_DATA) {
                        qWarning("1: Error writing file, contents not saved");
                        saveFinalize();
                        return false;
              }

           }
        }

	if (saveFinalize() == PWERR_DATA) {
                qWarning("2: Error writing file, contents not saved");
		return false;
	} else {
#ifndef DESKTOP
                Global::statusMessage (tr("Password file saved."));
#endif
                modified = false;
		return true;
	}
}

PasswordForm *newPwdDialog;
bool newPwdDialogResult = false;
void ZSafe::setPasswordDialogDone()
{
   newPwdDialogResult = true;
   newPwdDialog->close();
}

void ZSafe::getDocPassword(QString title)
{
qWarning ("getDocPassword");
    // open the 'Password' dialog
    PasswordForm *dialog = new PasswordForm(this, title, TRUE);
    newPwdDialog = dialog;
    newPwdDialogResult = false;

    connect( dialog->PasswordField, SIGNAL( returnPressed() ),
             this, SLOT( setPasswordDialogDone() ) );

    // CS: !!!
    // int pos = filename.findRev ('/');
    QString ti = filename.right (filename.length() - filename.findRev ('/') - 1);
#ifdef WIN32
    dialog->setCaption("Qt " + ti);
#else
    dialog->setCaption(ti);
#endif
    // dialog->setCaption(title);

    dialog->PasswordField->setFocus();
    DialogCode result = (DialogCode) dialog->exec();
#ifdef DESKTOP
       result = Accepted;
#endif

    QString password;
    if (result == Accepted || newPwdDialogResult)
    {
       m_password = dialog->PasswordField->text();
    }
    else
    {
       exitZs (1);
    }
}

int ZSafe::saveInit(const char *_filename, const char *password)
{
	char key[128];
	unsigned int j = 0;
	unsigned int keylength;
	// int val;
	int count2;
	Krc2* krc2 = new Krc2();

	/* first we should check the permissions of the filename */
/*
	if (QFile::exists(_filename)) {
		val = checkFile(_filename);
		if (val != PWERR_GOOD)
			return val;
	} else 
        {
		val = creat (_filename, (S_IRUSR | S_IWUSR));
		if (val == -1)
			return PWERR_OPEN;
		else
			close(val);
	}
*/
        QFileInfo f (_filename);
        save_buffer_length = f.size();
        save_buffer_length = ((save_buffer_length / 1024)+1) * 1024;

	fd = fopen (_filename, "wb");
	if (fd == NULL)
		return PWERR_OPEN;
	
	buffer = (char*)malloc(save_buffer_length);

	/* make the key ready */
	for (j = 0; password[j] != '\0'; j++) {
		key[j] = password[j];
	}
	keylength = j;
	krc2->rc2_expandkey (key, keylength, 128);

	/* First, we make the IV */
	for (count2 = 0; count2 < 4; count2++) {
		iv[count2] = rand ();
		putc ((unsigned char) (iv[count2] >> 8), fd);
		putc ((unsigned char) (iv[count2] & 0xff), fd);
	}

	bufferIndex = 0;
	return PWERR_GOOD;
}


int ZSafe::saveEntry(char *entry[FIELD_SIZE])
{
	char *text1;
	int count2, count3;
	unsigned short ciphertext[4];
	Krc2* krc2 = new Krc2();

	buffer = (char*)memset(buffer, '\0', save_buffer_length);

	for (count2 = 0; count2 < FIELD_SIZE; count2++) {
		text1 = entry[count2];
		if (strlen (text1) == 0) {
			strncat(buffer, " ", strlen(" "));
		} else {
			strncat(buffer, text1, strlen(text1));
		}
		/* Use 255 as the marker.  \n is too tough to test for */
		buffer[strlen (buffer)] = 255;
	} /*for (count2 = 0; count2 < 5; count2++)*/
	count2 = 0;
	/* I'm using CBC mode and encrypting the data straight from top down.
	 * At the bottom, encrypted, I will append an MD5 hash of the file, eventually.
	 * PKCS 5 padding (explained at the code section
	 */
	while (count2 < (int)strlen (buffer)) {
#ifndef WORDS_BIGENDIAN
		plaintext[bufferIndex] = buffer[count2 + 1] << 8;
		plaintext[bufferIndex] += buffer[count2] & 0xff;
#endif
#ifdef WORDS_BIGENDIAN
		plaintext[bufferIndex] = buffer[count2] << 8;
		plaintext[bufferIndex] += buffer[count2 + 1] & 0xff;
#endif
		bufferIndex++;
		if (bufferIndex == 4) {
			krc2->rc2_encrypt (plaintext);

			for (count3 = 0; count3 < 4; count3++) {
				ciphertext[count3] = iv[count3] ^ plaintext[count3];

				/* Now store the ciphertext as the iv */
				iv[count3] = plaintext[count3];

				/* reset the buffer index */
				bufferIndex = 0;
				if (putc ((unsigned char) (ciphertext[count3] >> 8), fd) == EOF) return PWERR_DATA;
				if (putc ((unsigned char) (ciphertext[count3] & 0xff), fd) == EOF) return PWERR_DATA;
			} /*for (count3 = 0; count3 < 5; count3++)*/
		} /*if (bufferIndex == 5)*/
		/* increment a short, not a byte */
		count2 += 2;
	} /*while (count2 < strlen (buffer))*/
        int ret = PWERR_GOOD;
	return ret;
}

int ZSafe::saveFinalize(void)
{
	int count1, retval = PWERR_GOOD;
	unsigned short ciphertext[4];
	Krc2* krc2 = new Krc2();

	/* Tack on the PKCS 5 padding
	 * How it works is we fill up the last n bytes with the value n
	 *
	 * So, if we have, say, 13 bytes, 8 of which are used, we have 5 left
	 * over, leaving us 3 short, so we fill it in with 3's.
	 *
	 * If we come out even, we fill it with 8 8s
	 *
	 * um, except that in this instance we are using 4 shorts instead of 8 bytes.
	 * so, half everything
	 */
	for (count1 = bufferIndex; count1 < 4; count1++) {
		plaintext[count1] = (4 - bufferIndex);
	}
	krc2->rc2_encrypt (plaintext);
	for (count1 = 0; count1 < 4; count1++) {
		ciphertext[count1] = iv[count1] ^ plaintext[count1];
		if (putc ((unsigned char) (ciphertext[count1] >> 8), fd) == EOF) retval = PWERR_DATA;
		if (putc ((unsigned char) (ciphertext[count1] & 0xff), fd) == EOF) retval = PWERR_DATA;
	}

	fclose (fd);
	free(buffer);
	return retval;
}

void ZSafe::quitMe ()
{
    qWarning ("QUIT...");

    if (modified)
    {
      switch( QMessageBox::information( this, tr("ZSafe"),
                                      tr("Do you want to save\nbefore exiting?"),
                                      tr("&Save"), 
                                      tr("S&ave with\nnew\npassword"),
                                      tr("&Don't Save"), 
                                      0      // Enter == button 0
                                      ) ) 
      { // Escape == button 2
      case 0: // Save clicked, Alt-S or Enter pressed.
        // save
        modified = false;
        saveDocument(filename, FALSE);
        exitZs (1);
        break;
      case 1: // 
        // Save with new password
        modified = false;
        saveDocument(filename, TRUE);
        exitZs (1);
        break;
      case 2: // Don't Save clicked or Alt-D pressed
        // don't save but exitZs
        exitZs (1);
        break;
      }
    }
    exitZs (1);

}

void ZSafe::categoryFieldActivated( const QString& category)
{
   if (categoryDialog)
      setCategoryDialogFields(categoryDialog, category);
}

void ZSafe::addCategory()
{
    if (filename.isEmpty())
    {
       QMessageBox::critical( 0, tr("ZSafe"),
         tr("No document defined.\nYou have to create a new document"));
       return;
    }
    else
    {
       // open the 'Category' dialog
        bool initIcons = false;
       // open the 'Category' dialog
        CategoryDialog *dialog;
        if (categoryDialog)
        {
           dialog = categoryDialog;
        }
        else
        {
           categoryDialog = new CategoryDialog(this, tr("Category"), TRUE);
#ifdef WIN32
           categoryDialog->setCaption ("Qt " + tr("Category"));
#endif
           dialog = categoryDialog;
           connect( dialog->CategoryField, 
                    SIGNAL( activated ( const QString &)),
                    this, SLOT( categoryFieldActivated( const QString & ) ) );
           initIcons = true;
        }

#ifdef DESKTOP
#ifndef WIN32
        QStringList list = conf->entryList( APP_KEY+"/fieldDefs" );
#else
        // read all categories from the config file and store
        // into a list
        QFile f (cfgFile);
        QStringList list;
        if ( f.open(IO_ReadOnly) ) {    // file opened successfully
           QTextStream t( &f );        // use a text stream
           QString s;
           int n = 1;
           while ( !t.eof() ) {        // until end of file...
               s = t.readLine();       // line of text excluding '\n'
               list.append(s);
           }
           f.close();
        }
#endif
#else           
        // read all categories from the config file and store
        // into a list
        QFile f (cfgFile);
        QStringList list;
        if ( f.open(IO_ReadOnly) ) {    // file opened successfully
           QTextStream t( &f );        // use a text stream
           QString s;
           while ( !t.eof() ) {        // until end of file...
               s = t.readLine();       // line of text excluding '\n'
               list.append(s);
           }
           f.close();
        }
#endif
        QStringList::Iterator it = list.begin();
        QString categ;
        QString firstCategory;
        dialog->CategoryField->clear(); // remove all items
        while( it != list.end() ) 
        {
           QString *cat = new QString (*it);
           if (cat->contains("-field1", FALSE))
           { 
#ifdef DESKTOP
#ifndef WIN32
              categ = cat->section ("-field1", 0, 0);
#else
              int pos = cat->find ("-field1");
			  categ = cat->left (pos);
#endif
#else
              int pos = cat->find ("-field1");
              cat->truncate(pos);
              categ = *cat;
#endif
              if (!categ.isEmpty())
              {
                 dialog->CategoryField->insertItem (categ, -1);
                 if (firstCategory.isEmpty())
                    firstCategory = categ;
              }
           }
           ++it;
        }


        if (firstCategory.isEmpty())
           setCategoryDialogFields(dialog);
        else
           setCategoryDialogFields(dialog, firstCategory);

        // CategoryDialog *dialog = new CategoryDialog(this, "Category", TRUE);

           if (initIcons)
           {
		Wait waitDialog(this, tr("Wait dialog"));
		waitDialog.waitLabel->setText(tr("Gathering icons..."));
		waitDialog.show();
		qApp->processEvents();

#ifdef DESKTOP
		QDir d(iconPath);
#else
		QDir d(QPEApplication::qpeDir() + "/pics/");
#endif
		d.setFilter( QDir::Files);

		const QFileInfoList *list = d.entryInfoList();
		QFileInfoListIterator it( *list );      // create list iterator
		QFileInfo *fi;                          // pointer for traversing

		dialog->IconField->insertItem("predefined");
		while ( (fi=it.current()) ) {           // for each file...
			QString fileName = fi->fileName();
			if(fileName.right(4) == ".png"){
				fileName = fileName.mid(0,fileName.length()-4);
#ifdef DESKTOP
				QPixmap imageOfFile;
                                imageOfFile.load(iconPath + fi->fileName());
#else
				QPixmap imageOfFile(Resource::loadPixmap(fileName));
#endif
				QImage foo = imageOfFile.convertToImage();
				foo = foo.smoothScale(16,16);
				imageOfFile.convertFromImage(foo);
				dialog->IconField->insertItem(imageOfFile,fileName);
			}
			++it;
		}
		waitDialog.hide();
           }
	
#ifndef WIN32
        dialog->show();
#endif
#ifndef DESKTOP
        // dialog->move (20, 100);
#endif
        DialogCode result = (DialogCode) dialog->exec();
#ifdef DESKTOP
       result = Accepted;
#endif

        QString category;
        QString icon;
        QString fullIconPath;
        QPixmap *pix;
        if (result == Accepted)
        {
           modified = true;
           category = dialog->CategoryField->currentText();
           icon = dialog->IconField->currentText()+".png";

           qWarning (category);

           QListViewItem *li = new ShadedListItem( 1, ListView );
           Category *c1 = new Category();
           c1->setCategoryName(category);

           // if (!icon.isEmpty() && !icon.isNull())
           if (icon != "predefined.png")
           {
               // build the full path
               fullIconPath = iconPath + icon;
               pix = new QPixmap (fullIconPath);
               // pix->resize(14, 14); 
               if (pix)
               {
                  // save the full pixmap name into the config file
// #ifndef WIN32
                  conf->writeEntry(APP_KEY+category, icon);
// #endif
                  saveConf();
                  QImage img = pix->convertToImage();
                  pix->convertFromImage(img.smoothScale(14,14));
                  c1->setIcon (*pix);
                  c1->setIconName(icon);
               }
               else
               {
                  QPixmap folder( ( const char** ) general_data );
                  c1->setIcon (folder);
               }
           }
           else
           {
              c1->setIcon (*getPredefinedIcon(category));
           }

           c1->setListItem (li);
           c1->initListItem();
           categories.insert (c1->getCategoryName(), c1);

           saveCategoryDialogFields(dialog);
        }
        else
        {
           // delete dialog;
           dialog->hide();
           return;
        }

    }

}

void ZSafe::delCategory()
{
    if (!selectedItem)
       return;
    if (isCategory(selectedItem))
    {
       switch( QMessageBox::information( this, tr("ZSafe"),
                                      tr("Do you want to delete?"),
                                      tr("&Delete"), tr("D&on't Delete"), 
                                      0      // Enter == button 0
                                      ) ) { // Escape == button 2
       case 0: // Delete clicked, Alt-S or Enter pressed.
          // Delete from the category list
          modified = true;
          categories.remove (selectedItem->text(0));
// #ifndef WIN32
          conf->removeEntry (selectedItem->text(0));
// #endif
          saveConf();

          // Delete the selected item and all subitems
          // step through all subitems
          QListViewItem *si;
          for (si = selectedItem->firstChild();
               si != NULL; )
          {
              QListViewItem *_si = si;
              si = si->nextSibling();
              selectedItem->takeItem(_si); // remove from view list
              if (_si) delete _si;
          }
          ListView->takeItem(selectedItem);
          delete selectedItem;

          selectedItem = NULL;
          break;
       case 1: // Don't delete
          break;
       }

    }
}

void ZSafe::setCategoryDialogFields(CategoryDialog *dialog)
{
   if (!dialog)
      return;

   QString icon;
   if (selectedItem) 
   {
      dialog->Field1->setText(getFieldLabel (selectedItem, "1", tr("Name")));
      dialog->Field2->setText(getFieldLabel (selectedItem, "2", tr("Username")));
      dialog->Field3->setText(getFieldLabel (selectedItem, "3", tr("Password")));
      dialog->Field4->setText(getFieldLabel (selectedItem, "4", tr("Comment")));
      dialog->Field5->setText(getFieldLabel (selectedItem, "5", tr("Field 4")));
      dialog->Field6->setText(getFieldLabel (selectedItem, "6", tr("Field 5")));

      Category *cat= categories.find (selectedItem->text(0));
      if (cat)
      {
         icon = cat->getIconName();
      }
      else
         icon = conf->readEntry(APP_KEY+selectedItem->text(0));
   }
   else
   {
      dialog->Field1->setText(tr("Name"));
      dialog->Field2->setText(tr("Username"));
      dialog->Field3->setText(tr("Password"));
      dialog->Field4->setText(tr("Comment"));
      dialog->Field5->setText(tr("Field 4"));
      dialog->Field6->setText(tr("Field 5"));
   }

#ifdef DESKTOP
	QDir d(iconPath);
#else
	QDir d(QPEApplication::qpeDir() + "/pics/");
#endif
	d.setFilter( QDir::Files);

	const QFileInfoList *list = d.entryInfoList();
	int i=0;
	QFileInfoListIterator it( *list );      // create list iterator
	QFileInfo *fi;                          // pointer for traversing
   if (icon.isEmpty() || icon.isNull())
   {
      dialog->IconField->setCurrentItem(0);
   }
   else
   {
		while ( (fi=it.current()) ) 
      { // for each file...
			QString fileName = fi->fileName();
			if(fileName.right(4) == ".png")
                        {
			fileName = fileName.mid(0,fileName.length()-4);

			if(fileName+".png"==icon) 
         {
            dialog->IconField->setCurrentItem(i+1);
            break;
         }
			   ++i;
			}
			++it;
		}
	}
}

void ZSafe::setCategoryDialogFields(CategoryDialog *dialog, QString category)
{
   if (!dialog)
      return;

   dialog->Field1->setText(getFieldLabel (category, "1", tr("Name")));
   dialog->Field2->setText(getFieldLabel (category, "2", tr("Username")));
   dialog->Field3->setText(getFieldLabel (category, "3", tr("Password")));
   dialog->Field4->setText(getFieldLabel (category, "4", tr("Comment")));
   dialog->Field5->setText(getFieldLabel (category, "5", tr("Field 4")));
   dialog->Field6->setText(getFieldLabel (category, "6", tr("Field 5")));

   QString icon;
   Category *cat= categories.find (category);
   if (cat)
   {
      icon = cat->getIconName();
   }
   else
      icon = conf->readEntry(APP_KEY+category);

#ifdef DESKTOP
	QDir d(iconPath);
#else
	QDir d(QPEApplication::qpeDir() + "/pics/");
#endif
	d.setFilter( QDir::Files);

	const QFileInfoList *list = d.entryInfoList();
	int i=0;
	QFileInfoListIterator it( *list );      // create list iterator
	QFileInfo *fi;                          // pointer for traversing
   if (icon.isEmpty() || icon.isNull())
   {
      dialog->IconField->setCurrentItem(0);
   }
   else
   {
		while ( (fi=it.current()) ) 
      { // for each file...
			QString fileName = fi->fileName();
			if(fileName.right(4) == ".png")
                        {
			fileName = fileName.mid(0,fileName.length()-4);

			if(fileName+".png"==icon) 
         {
            dialog->IconField->setCurrentItem(i+1);
            break;
         }
			   ++i;
			}
			++it;
		}
	}
}

void ZSafe::saveCategoryDialogFields(CategoryDialog *dialog)
{
   QString app_key = APP_KEY;
#ifndef DESKTOP
   conf->setGroup ("fieldDefs");
#else
#ifndef WIN32
   app_key += "/fieldDefs/";
#endif
#endif
   QString category = dialog->CategoryField->currentText();
// #ifndef WIN32
   conf->writeEntry(app_key+category+"-field1", dialog->Field1->text());
   conf->writeEntry(app_key+category+"-field2", dialog->Field2->text());
   conf->writeEntry(app_key+category+"-field3", dialog->Field3->text());
   conf->writeEntry(app_key+category+"-field4", dialog->Field4->text());
   conf->writeEntry(app_key+category+"-field5", dialog->Field5->text());
   conf->writeEntry(app_key+category+"-field6", dialog->Field6->text());
// #endif
   saveConf();
#ifndef DESKTOP
   conf->setGroup ("zsafe");
#endif
}

void ZSafe::editCategory()
{
    if (!selectedItem)
       return;
    if (isCategory(selectedItem))
    {
        QString category = selectedItem->text(0);
        bool initIcons = false;
        // open the 'Category' dialog
        CategoryDialog *dialog;
        if (categoryDialog)
        {
           dialog = categoryDialog;
        }
        else
        {
           categoryDialog = new CategoryDialog(this, tr("Category"), TRUE);
#ifdef WIN32
           categoryDialog->setCaption ("Qt " + tr("Category"));
#endif
           dialog = categoryDialog;
           connect( dialog->CategoryField, 
                    SIGNAL( activated ( const QString &)),
                    this, SLOT( categoryFieldActivated( const QString & ) ) );
           initIcons = true;
        }
        setCategoryDialogFields(dialog);

#ifdef DESKTOP
#ifndef WIN32
        QStringList list = conf->entryList( APP_KEY+"/fieldDefs" );
#else
        // read all categories from the config file and store
        // into a list
        QFile f (cfgFile);
        QStringList list;
        if ( f.open(IO_ReadOnly) ) {    // file opened successfully
           QTextStream t( &f );        // use a text stream
           QString s;
           int n = 1;
           while ( !t.eof() ) {        // until end of file...
               s = t.readLine();       // line of text excluding '\n'
               list.append(s);
           }
           f.close();
        }
#endif
#else
        // read all categories from the config file and store
        // into a list
        QFile f (cfgFile);
        QStringList list;
        if ( f.open(IO_ReadOnly) ) {    // file opened successfully
           QTextStream t( &f );        // use a text stream
           QString s;
           while ( !t.eof() ) {        // until end of file...
               s = t.readLine();       // line of text excluding '\n'
               list.append(s);
           }
           f.close();
        }
#endif
        QStringList::Iterator it = list.begin();
        QString categ;
        dialog->CategoryField->clear(); // remove all items
        int i=0;
        bool foundCategory = false;
        while( it != list.end() ) 
        {
           QString *cat = new QString (*it);
           if (cat->contains("-field1", FALSE))
           { 
#ifdef DESKTOP
#ifndef WIN32
              categ = cat->section ("-field1", 0, 0);
#else
	          int pos = cat->find ("-field1");
			  categ = cat->left (pos);
#endif
#else
              int pos = cat->find ("-field1");
              cat->truncate(pos);
              categ = *cat;
#endif
              if (!categ.isEmpty())
              {
                 dialog->CategoryField->insertItem (categ, i);
                 if (category.compare(categ) == 0)
                 {
                     dialog->CategoryField->setCurrentItem(i);
                     foundCategory = true;
                 }
                 i++;
              }
           }
           ++it;
        }
        if (!foundCategory)
        {
            dialog->CategoryField->insertItem (category, i);
            dialog->CategoryField->setCurrentItem(i);
        }

        QString icon;
        Category *cat= categories.find (selectedItem->text(0));
        if (cat)
        {
           icon = cat->getIconName();
        }

           if (initIcons)
           {

		Wait waitDialog(this, tr("Wait dialog"));
		waitDialog.waitLabel->setText(tr("Gathering icons..."));
		waitDialog.show();
		qApp->processEvents();

#ifdef DESKTOP
		QDir d(iconPath);
#else
		QDir d(QPEApplication::qpeDir() + "/pics/");
#endif
		d.setFilter( QDir::Files);

		const QFileInfoList *list = d.entryInfoList();
		int i=0;
		QFileInfoListIterator it( *list );      // create list iterator
		QFileInfo *fi;                          // pointer for traversing
                if (icon.isEmpty() || icon.isNull())
                {
                   dialog->IconField->setCurrentItem(0);
                }

		dialog->IconField->insertItem("predefined");
		while ( (fi=it.current()) ) {           // for each file...
			QString fileName = fi->fileName();
			if(fileName.right(4) == ".png")
                        {
				fileName = fileName.mid(0,fileName.length()-4);
#ifdef DESKTOP
				QPixmap imageOfFile;
                                imageOfFile.load(iconPath + fi->fileName());
#else
				QPixmap imageOfFile(Resource::loadPixmap(fileName));
#endif
				QImage foo = imageOfFile.convertToImage();
				foo = foo.smoothScale(16,16);
				imageOfFile.convertFromImage(foo);
				dialog->IconField->insertItem(imageOfFile,fileName);
				if(fileName+".png"==icon) 
                                   dialog->IconField->setCurrentItem(i+1);
			        ++i;
			}
			++it;
		}
		waitDialog.hide();
           }
           else
           {
#ifdef DESKTOP
		// QDir d(QDir::homeDirPath() + "/pics/");
		QDir d(iconPath);
#else
		QDir d(QPEApplication::qpeDir() + "/pics/");
#endif
		d.setFilter( QDir::Files);

		const QFileInfoList *list = d.entryInfoList();
		int i=0;
		QFileInfoListIterator it( *list );      // create list iterator
		QFileInfo *fi;                          // pointer for traversing
                if (icon.isEmpty() || icon.isNull())
                {
                   dialog->IconField->setCurrentItem(0);
                }
                else
                {

		  while ( (fi=it.current()) ) 
                  { // for each file...
			QString fileName = fi->fileName();
			if(fileName.right(4) == ".png")
                        {
				fileName = fileName.mid(0,fileName.length()-4);


				if(fileName+".png"==icon) 
                                {
                                   dialog->IconField->setCurrentItem(i+1);
                                   break;
                                }
			        ++i;
			}
			++it;
		  }
		}
           }

        // dialog->show();
#ifndef DESKTOP
        // dialog->move (20, 100);
#endif
        DialogCode result = (DialogCode) dialog->exec();
#ifdef DESKTOP
       result = Accepted;
#endif

        QString fullIconPath;
        QPixmap *pix;
        if (result == Accepted)
        {
           modified = true;
           if (category != dialog->CategoryField->currentText())
           {
              categories.remove (category);
// #ifndef WIN32
              conf->removeEntry(category);
// #endif
              saveConf();
           }

           category = dialog->CategoryField->currentText();
           icon = dialog->IconField->currentText()+".png";

           if (cat)
           {
              qWarning("Category found");

              // if (!icon.isEmpty() && !icon.isNull())
              if (icon != "predefined.png")
              {
                  // build the full path
                  fullIconPath = iconPath + icon;
                  pix = new QPixmap (fullIconPath);
                  if (pix)
                  {
                     // save the full pixmap name into the config file
// #ifndef WIN32
                     conf->writeEntry(APP_KEY+category, icon);
// #endif
                     saveConf();
                     QImage img = pix->convertToImage();
                     pix->convertFromImage(img.smoothScale(14,14));
                     cat->setIconName (icon);
                     cat->setIcon (*pix);
                  }
              }
              else
              {
// #ifndef WIN32
                 conf->removeEntry (category);
// #endif
                 saveConf();
                 cat->setIcon (*getPredefinedIcon(category));
              }

              // change the category name of the selected category
              QListViewItem *catItem = cat->getListItem();
              if (catItem)
              {
                 qWarning (category);
#ifdef JPATCH_HDE
                 catItem->setText( 0, category );
                 cat->setCategoryName (category);
#else
                 catItem->setText( 0, tr( category ) );
                 cat->setCategoryName (tr(category));
#endif
                 cat->initListItem();
                 categories.insert (category, cat);
              }
           }
           saveCategoryDialogFields(dialog);
        }
        else
        {
           // delete dialog;
           dialog->hide();
           return;
        }

    }
}

void ZSafe::cutItem()
{
    if (!selectedItem)
       return;
    if (!isCategory(selectedItem))
    {
       IsCut = true;
       copiedItem = selectedItem;
    }
}

void ZSafe::copyItem()
{
    if (!selectedItem)
       return;
    if (!isCategory(selectedItem))
    {
       IsCopy = true;
       copiedItem = selectedItem;
    }
}

// paste item into category
void ZSafe::pasteItem()
{
    if (!selectedItem)
       return;
    if (isCategory(selectedItem))
    {
       modified = true;
       if (IsCut)
       {
          if (copiedItem)
          {
             // add the new item
             QListViewItem *i = new ShadedListItem (0, selectedItem);
             // i->setOpen (TRUE);
             i->setText (0, copiedItem->text(0));
             i->setText (1, copiedItem->text(1));
             i->setText (2, copiedItem->text(2));
             i->setText (3, copiedItem->text(3));
             i->setText (4, copiedItem->text(4));
             i->setText (5, copiedItem->text(5));
             selectedItem->setOpen( TRUE );

             // remove the cutted item
             copiedItem->parent()->takeItem(copiedItem);
             selectedItem = NULL;
          }
       }
       else if (IsCopy)
       {
          if (copiedItem)
          {
              // add the new item
              QListViewItem *i = new ShadedListItem (0, selectedItem);
              // i->setOpen (TRUE);
              i->setText (0, copiedItem->text(0));
              i->setText (1, copiedItem->text(1));
              i->setText (2, copiedItem->text(2));
              i->setText (3, copiedItem->text(3));
              i->setText (4, copiedItem->text(4));
              i->setText (5, copiedItem->text(5));
              selectedItem->setOpen( TRUE );
          }
       }
    }
    IsCut = false;
    IsCopy = false;
}

void ZSafe::newDocument()
{

    // open the file dialog
#ifndef DESKTOP
#ifndef NO_OPIE
   QMap<QString, QStringList> mimeTypes;
   mimeTypes.insert(tr("All"), QStringList() );
   mimeTypes.insert(tr("ZSafe"), "zsafe/*" );
   QString newFile = OFileDialog::getSaveFileName( OFileSelector::EXTENDED_ALL,
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       QString::null,
                       mimeTypes,
                       this,
                       tr ("Create new ZSafe document"));
#else
   QString newFile = ScQtFileEdit::getSaveAsFileName(this, 
                       tr ("Create new ZSafe document"),
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       "*.zsf");
#endif
#else
    QString newFile = QFileDialog::getSaveFileName(
	            QDir::homeDirPath() + "/Documents/application/zsafe",
                    "ZSafe (*.zsf)",
                    this,
                    "ZSafe File Dialog"
                    "Choose a ZSafe file" );
#endif

    // open the new document
    if (newFile && newFile.length() > 0 )
    {
       // save the previous opened document
       if (!filename.isEmpty())
          saveDocument(filename, FALSE);

       modified = true;

       // clear the password list
       QListViewItem *i;
       QListViewItem *c = NULL;
       // step through all categories
       for (i = ListView->firstChild();
            i != NULL;
            i = i->nextSibling())
       {
          if (c) delete c; // delete the previous category

          c = i;
          // step through all subitems
          QListViewItem *si;
          for (si = i->firstChild();
               si != NULL; )
          {
              QListViewItem *_si = si;
              si = si->nextSibling();
              i->takeItem(_si); // remove from view list
              if (_si) delete _si;
          }
       }
       if (c) delete c; // delete the previous category
       categories.clear();

       // m_password = "";
       selectedItem = NULL;

       filename = newFile;

       // save the current filename to the config file
       conf->writeEntry(APP_KEY+"document", filename);
       saveConf();
       QString ti = filename.right (filename.length() - filename.findRev ('/') - 1);
#ifdef WIN32
       this->setCaption("Qt ZSafe: " + ti);
#else
       this->setCaption("ZSafe: " + ti);
#endif

       // openDocument(filename);

       QMessageBox::information( this, tr("ZSafe"), 
       tr("Now you have to enter\na password twice for your\nnewly created document."),       tr("&OK"), 0);

       saveDocumentWithPwd();
    }
}

void ZSafe::loadDocument()
{

    // open the file dialog
#ifndef DESKTOP
#ifndef NO_OPIE
   QMap<QString, QStringList> mimeTypes;
   mimeTypes.insert(tr("All"), QStringList() );
   mimeTypes.insert(tr("ZSafe"), "zsafe/*" );
   QString newFile = OFileDialog::getOpenFileName( OFileSelector::EXTENDED_ALL,
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       QString::null,
                       mimeTypes,
                       this,
                       tr ("Open ZSafe document"));
#else
   QString newFile = ScQtFileEdit::getOpenFileName(this, 
                       tr ("Open ZSafe document"),
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       "*.zsf");
#endif
#else
    QString newFile = QFileDialog::getOpenFileName(
	            QDir::homeDirPath() + "/Documents/application/zsafe",
                    "ZSafe (*.zsf)",
                    this,
                    "ZSafe File Dialog"
                    "Choose a ZSafe file" );
#endif

    // open the new document
    if (newFile && newFile.length() > 0 )
    {
       // save the previous opened document
       if (!filename.isEmpty())
          saveDocument(filename, FALSE);

       // clear the password list
       QListViewItem *i;
       QListViewItem *c = NULL;
       // step through all categories
       for (i = ListView->firstChild();
            i != NULL;
            i = i->nextSibling())
       {
          if (c) delete c; // delete the previous category

          c = i;
          // step through all subitems
          QListViewItem *si;
          for (si = i->firstChild();
               si != NULL; )
          {
              QListViewItem *_si = si;
              si = si->nextSibling();
              i->takeItem(_si); // remove from view list
              if (_si) delete _si;
          }
       }
       if (c) delete c; // delete the previous category
       categories.clear();
       m_password = "";
       selectedItem = NULL;
       filename = newFile;

       // save the current filename to the config file
       conf->writeEntry(APP_KEY+"document", filename);
       saveConf();
       QString ti = filename.right (filename.length() - filename.findRev ('/') - 1);
#ifdef WIN32
       this->setCaption("Qt ZSafe: " + ti);
#else
       this->setCaption("ZSafe: " + ti);
#endif

       openDocument(filename);
    }
}

void ZSafe::saveDocumentAs()
{

#ifndef DESKTOP
#ifndef NO_OPIE
   QMap<QString, QStringList> mimeTypes;
   mimeTypes.insert(tr("All"), QStringList() );
   mimeTypes.insert(tr("ZSafe"), "zsafe/*" );
   QString newFile = OFileDialog::getSaveFileName( OFileSelector::EXTENDED_ALL,
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       QString::null,
                       mimeTypes,
                       this,
                       tr ("Save ZSafe document as.."));
#else
   QString newFile = ScQtFileEdit::getSaveAsFileName(this, 
                       tr ("Save ZSafe document as.."),
                       QDir::homeDirPath() + "/Documents/application/zsafe",
                       "*.zsf");
#endif
#else
    // open the file dialog
    QString newFile = QFileDialog::getSaveFileName(
	            QDir::homeDirPath() + "/Documents/application/zsafe",
                    "ZSafe (*.zsf)",
                    this,
                    "ZSafe File Dialog"
                    "Choose a ZSafe file" );
#endif

    // open the new document
    if (newFile && newFile.length() > 0 )
    {
       // save the previous opened document
       if (!filename.isEmpty())
          saveDocument(filename, FALSE);

       selectedItem = NULL;
       filename = newFile;

       // save the current filename to the config file
       conf->writeEntry(APP_KEY+"document", filename);
       saveConf();
       QString ti = filename.right (filename.length() - filename.findRev ('/') - 1);
#ifdef WIN32
       this->setCaption("Qt ZSafe: " + ti);
#else
       this->setCaption("ZSafe: " + ti);
#endif

       QMessageBox::information( this, tr("ZSafe"), 
       tr("Now you have to enter\na password twice for your\nnewly created document."),       tr("&OK"), 0);

       saveDocumentWithPwd();
    }
}

void ZSafe::saveDocumentWithoutPwd()
{
   saveDocument(filename, FALSE);
}

void ZSafe::saveDocumentWithPwd()
{
   saveDocument(filename, TRUE);
}

void ZSafe::about()
{
   QString info;
#ifdef JPATCH_HDE
   info  = "<html><body><div align=""center"">";
   info += "<b>";
   info += tr("Zaurus Password Manager<br>");
   info += tr("ZSafe version 2.1.2-jv01b<br>");
   info += "</b>";
   info += tr("by Carsten Schneider<br>");
   info += "zcarsten@gmx.net<br>";
   info += "http://z-soft.z-portal.info/zsafe";
   info += "<br>";
   info += tr("Translations by Robert Ernst<br>");
   info += "robert.ernst@linux-solutions.at<br>";

   info += "<br><br>";
   info += QString::fromUtf8("æ~W¥æ~\\¬èª~^/VGA Zauruså¯¾å¿~\\ã~C~Qã~C~Cã~C~Aä½~\\æ ~H~P<br>");
   info += "HADECO R&D<br>";
   info += "r&d@hadeco.co.jp<br>";
   info += "http://www.hadeco.co.jp/r&d/<br>";
   info += "<br></div>";
   info += "</body></html>";
#else
   info  = "<html><body><div align=""center"">";
   info += "<b>";
   info += tr("Zaurus Password Manager<br>");
   info += tr("ZSafe version 2.1.2<br>");
   info += "</b>";
   info += tr("by Carsten Schneider<br>");
   info += "zcarsten@gmx.net<br>";
   info += "http://z-soft.z-portal.info/zsafe";
   info += "<br>";
   info += tr("Translations by Robert Ernst<br>");
   info += "robert.ernst@linux-solutions.at<br>";
   info += "<br></div>";
   info += "</body></html>";
#endif

   // QMessageBox::information( this, tr("ZSafe"), info, tr("&OK"), 0);

   QMessageBox mb( this, tr("ZSafe"));
   mb.setText (info);
   mb.setButtonText (QMessageBox::Ok, tr ("&OK"));
   QPixmap zsafe_img((const char**) zsafe_xpm);
   mb.setIconPixmap (zsafe_img);
   mb.exec();
}

void ZSafe::setExpandFlag()
{
   expandTree = !expandTree;
   file->setItemChecked('o', expandTree);
#ifndef DESKTOP
   conf->setGroup ("zsafePrefs");
#endif
// #ifndef WIN32
   conf->writeEntry (APP_KEY+"expandTree", expandTree);
// #endif
   saveConf();

}

void ZSafe::paintEvent( QPaintEvent * )
{
   if (raiseFlag)
   {
      raiseFlag = false;
      raiseTimer.start (1, true);
      if (infoForm->isVisible())
         infoForm->raise();
   }
}

void ZSafe::resizeEvent ( QResizeEvent * ) 
{
   // qWarning ("resizeEvent");
#ifndef DESKTOP
   DeskW = appl->desktop()->width();
   DeskH = appl->desktop()->height();
#else
   DeskW = this->width();
   DeskH = this->height();
#endif
   qWarning( QString("Width : %1").arg(DeskW), 2000 );
   qWarning( QString("Height: %1").arg(DeskH), 2000 );

   New->setGeometry   ( QRect( DeskW-84, 2, 20, 20 ) ); 
   Edit->setGeometry  ( QRect( DeskW-64, 2, 20, 20 ) ); 
   Delete->setGeometry( QRect( DeskW-44, 2, 20, 20 ) ); 
   Find->setGeometry  ( QRect( DeskW-24, 2, 20, 20 ) ); 
}

void ZSafe::slotRaiseTimer()
{
   if (infoForm->isVisible())
      infoForm->raise();
   raiseFlag = true;
}

QPixmap * ZSafe::getPredefinedIcon(QString category)
{
   QPixmap *pm;
   if (category == "Bank cards")
      pm = new QPixmap((const char**)bank_cards_data);
   else if (category == "Passwords")
      pm = new QPixmap((const char**)passwords_data);
   else if (category == "Software")
      pm = new QPixmap((const char**)software_data);
   else if (category == "General")
      pm = new QPixmap((const char**)general_data);
   else
      pm = new QPixmap((const char**)general_data);
   return pm;
}

void ZSafe::setDocument(const QString& fileref)
{
#ifndef DESKTOP
     // stop the timer to prevent loading of the default document
     docuTimer.stop();

     DocLnk link(fileref);
     if ( link.isValid() )
     {
         // if (filename != link.file())
            // saveDocument(filename, FALSE);
         filename = link.file();
     }
     else
     {
         // if (filename != fileref)
            // saveDocument(filename, FALSE);
         filename = fileref;
     }
     // save the current filename to the config file
     conf->writeEntry(APP_KEY+"document", filename);
     saveConf();
     QString ti = filename.right (filename.length() - filename.findRev ('/') - 1);
#ifdef WIN32
     this->setCaption("Qt ZSafe: " + ti);
#else
     this->setCaption("ZSafe: " + ti);
#endif

     // clear the password list
     QListViewItem *i;
     QListViewItem *c = NULL;
     // step through all categories
     for (i = ListView->firstChild();
            i != NULL;
            i = i->nextSibling())
     {
          if (c) delete c; // delete the previous category

          c = i;
          // step through all subitems
          QListViewItem *si;
          for (si = i->firstChild();
               si != NULL; )
          {
              QListViewItem *_si = si;
              si = si->nextSibling();
              i->takeItem(_si); // remove from view list
              if (_si) delete _si;
          }
     }
     if (c) delete c; // delete the previous category
     categories.clear();

     m_password = "";
     selectedItem = NULL;
     
     openDocument(filename);
#endif
}



