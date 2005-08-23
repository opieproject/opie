/****************************************************************************
** Form implementation generated from reading ui file 'obexsendbase.ui'
**
** Created: Fri Aug 5 00:20:45 2005
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "obexsendbase.h"

#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"14 14 4 1",
". c None",
"b c #000000",
"a c #c00000",
"# c #ff0000",
"..............",
"...#a.........",
"....###.......",
"......a##.....",
"..#a....a#....",
"...##a...a#...",
".....##...#...",
"..#a..a#..a#..",
"...##..#a..#..",
"....a#..#..#a.",
".bb..#a.#a..#.",
".bbb..#..#....",
".bbb..........",
".............."};

static const char* const image1_data[] = { 
"14 18 107 2",
"Qt c None",
".8 c #000000",
"#t c #000102",
"#K c #000104",
"#D c #00040c",
"#O c #00050f",
"#N c #000d24",
"#M c #000f2a",
"#L c #001130",
"#j c #00163d",
"#J c #001c52",
"#s c #002770",
"#C c #002772",
".F c #002d7f",
"#c c #00338f",
"#I c #003699",
"#B c #0038a3",
"#E c #003aa5",
"#A c #003cac",
"#r c #003cad",
"## c #003da7",
".4 c #003da9",
"#h c #003ead",
"#q c #003eb0",
".7 c #003fb1",
".N c #003fb3",
".V c #0040b2",
"#z c #0042b9",
".5 c #0042ba",
".U c #0043bb",
"#a c #0043bd",
"#F c #0043c0",
".W c #0044be",
"#w c #0044c4",
"#H c #0045c6",
"#m c #0046c7",
".L c #0048ca",
".M c #0049ce",
"#i c #0049d1",
"#v c #004ad6",
".6 c #004bd6",
".X c #004cd8",
"#G c #004cd9",
"#b c #004cda",
"#l c #004cdb",
".B c #004edf",
".D c #004fe1",
".C c #0050e4",
".E c #0052e9",
"#p c #013eab",
".Y c #0141b7",
".w c #014dda",
"#u c #014ede",
"#k c #0150e3",
".i c #0151e4",
".p c #0153ea",
".h c #0156f2",
".b c #0156f3",
".9 c #0256f1",
".g c #0256f2",
"#e c #054dd2",
".a c #0558f2",
".2 c #0659f3",
".Z c #075af2",
".S c #075af3",
".1 c #0b5df3",
".# c #0d5ff3",
".R c #0e60f4",
".x c #105fef",
".0 c #1061f2",
".O c #1263f4",
".G c #1464f1",
".c c #1563ed",
".3 c #1652bc",
".K c #165ad7",
".q c #1764ec",
".j c #1865ec",
".d c #1d69ee",
"#y c #1e59c6",
".J c #206df6",
".Q c #226ff6",
".T c #2365dd",
".y c #256fef",
".P c #2672f6",
"#x c #2967d8",
"#d c #296ded",
".k c #2a74f1",
".r c #2b75f2",
".H c #2d77f5",
".I c #347df9",
".l c #357ef7",
".z c #3780f8",
".t c #3f84f9",
"#o c #4876c8",
".s c #498af8",
".e c #4c88f5",
".o c #4c88f6",
"#g c #4e79c5",
".v c #4f8af7",
"#n c #5086e9",
"#f c #5087eb",
".f c #5890f7",
"#. c #5c90f1",
".u c #5d95f8",
".m c #5e95f8",
".A c #6b9df8",
".n c #ffffff",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQt.#.a.bQtQtQtQtQtQt",
"QtQtQt.c.d.e.f.g.h.iQtQtQtQt",
"QtQt.j.k.l.m.n.o.b.b.pQtQtQt",
"Qt.q.r.s.t.u.n.n.v.b.h.wQtQt",
".x.y.z.n.n.A.n.B.n.C.D.E.FQt",
".G.H.I.J.n.n.n.n.K.L.M.B.NQt",
".O.P.Q.R.S.n.n.T.U.V.W.X.YQt",
".Z.0.1.2.n.n.n.n.3.4.5.6.7.8",
".h.9.b.n.n#..n.B.n###a#b#cQt",
"Qt.b.b#d#e#f.n.n#g#h#a#i#jQt",
"Qt#k.b#l#m#n.n#o#p#q#r#s#tQt",
"QtQt#u#v#w#x#y#z#A#B#C#DQtQt",
"QtQtQt#E#F.B#G#H#I#J#KQtQtQt",
"QtQtQtQtQt#L#M#N#O.8QtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt"};


/* 
 *  Constructs a obexSendBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
obexSendBase::obexSendBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    if ( !name )
	setName( "obexSendBase" );
    resize( 363, 221 ); 
    setCaption( tr( "Send via OBEX" ) );
    obexSendBaseLayout = new QVBoxLayout( this ); 
    obexSendBaseLayout->setSpacing( 6 );
    obexSendBaseLayout->setMargin( 11 );

    Layout1 = new QHBoxLayout; 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    sendLabel = new QLabel( this, "sendLabel" );
    sendLabel->setText( tr( "Sending:" ) );
    Layout1->addWidget( sendLabel );

    fileToSend = new QLabel( this, "fileToSend" );
    fileToSend->setText( tr( "Unknown" ) );
    Layout1->addWidget( fileToSend );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );
    obexSendBaseLayout->addLayout( Layout1 );

    Layout4 = new QHBoxLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    irdaLabel = new QLabel( this, "irdaLabel" );
    irdaLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)1, irdaLabel->sizePolicy().hasHeightForWidth() ) );
    irdaLabel->setText( tr( "" ) );
    irdaLabel->setPixmap( image0 );
    Layout4->addWidget( irdaLabel );

    irdaStatus = new QLabel( this, "irdaStatus" );
    irdaStatus->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, irdaStatus->sizePolicy().hasHeightForWidth() ) );
    irdaStatus->setText( tr( "Unknown" ) );
    Layout4->addWidget( irdaStatus );

    btLabel = new QLabel( this, "btLabel" );
    btLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)1, btLabel->sizePolicy().hasHeightForWidth() ) );
    btLabel->setText( tr( "" ) );
    btLabel->setPixmap( image1 );
    Layout4->addWidget( btLabel );

    btStatus = new QLabel( this, "btStatus" );
    btStatus->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, btStatus->sizePolicy().hasHeightForWidth() ) );
    btStatus->setText( tr( "Unknown" ) );
    Layout4->addWidget( btStatus );
    obexSendBaseLayout->addLayout( Layout4 );

    receiverList = new QListView( this, "receiverList" );
    receiverList->addColumn( tr( "Receiver" ) );
    receiverList->header()->setClickEnabled( FALSE, receiverList->header()->count() - 1 );
    receiverList->header()->setResizeEnabled( FALSE, receiverList->header()->count() - 1 );
    receiverList->addColumn( tr( "T" ) );
    receiverList->header()->setClickEnabled( FALSE, receiverList->header()->count() - 1 );
    receiverList->header()->setResizeEnabled( FALSE, receiverList->header()->count() - 1 );
    receiverList->addColumn( tr( "S" ) );
    receiverList->header()->setClickEnabled( FALSE, receiverList->header()->count() - 1 );
    receiverList->header()->setResizeEnabled( FALSE, receiverList->header()->count() - 1 );
    receiverList->addColumn( tr( "Status" ) );
    receiverList->header()->setClickEnabled( FALSE, receiverList->header()->count() - 1 );
    receiverList->header()->setResizeEnabled( FALSE, receiverList->header()->count() - 1 );
    obexSendBaseLayout->addWidget( receiverList );

    Layout3 = new QHBoxLayout; 
    Layout3->setSpacing( 6 );
    Layout3->setMargin( 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer_2 );

    scanButton = new QPushButton( this, "scanButton" );
    scanButton->setText( tr( "Scan" ) );
    Layout3->addWidget( scanButton );

    sendButton = new QPushButton( this, "sendButton" );
    sendButton->setText( tr( "Send" ) );
    Layout3->addWidget( sendButton );

    doneButton = new QPushButton( this, "doneButton" );
    doneButton->setText( tr( "Done" ) );
    Layout3->addWidget( doneButton );
    obexSendBaseLayout->addLayout( Layout3 );

    // signals and slots connections
    connect( scanButton, SIGNAL( clicked() ), this, SLOT( scan_for_receivers() ) );
    connect( sendButton, SIGNAL( clicked() ), this, SLOT( send_to_receivers() ) );
    connect( doneButton, SIGNAL( clicked() ), this, SLOT( userDone() ) );
    connect( receiverList, SIGNAL( clicked(QListViewItem*) ), this, SLOT( toggle_receiver(QListViewItem *) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
obexSendBase::~obexSendBase()
{
    // no need to delete child widgets, Qt does it all for us
}

void obexSendBase::scan_for_receivers()
{
    qWarning( "obexSendBase::scan_for_receivers(): Not implemented yet!" );
}

void obexSendBase::send_to_receivers()
{
    qWarning( "obexSendBase::send_to_receivers(): Not implemented yet!" );
}

void obexSendBase::toggle_receiver(QListViewItem *)
{
    qWarning( "obexSendBase::toggle_receiver(QListViewItem *): Not implemented yet!" );
}

void obexSendBase::userDone()
{
    qWarning( "obexSendBase::userDone(): Not implemented yet!" );
}

