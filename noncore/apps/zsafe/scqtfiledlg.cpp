/****************************************************************************
** Form implementation generated from reading ui file 'scqtfiledlg.ui'
**
** Created: Sun Jun 8 15:51:12 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "scqtfiledlg.h"

#include <qcombobox.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"24 16 3 1",
". c None",
"# c #000040",
"a c #808080",
"........................",
"........................",
".................####...",
".................####...",
".......##........#aa#...",
"......###........#aa#...",
".....####........#aa#...",
"....##############aa#...",
"...###aaaaaaaaaaaaaa#...",
"..####aaaaaaaaaaaaaa#...",
"...###aaaaaaaaaaaaaa#...",
"....#################...",
".....####...............",
"......###...............",
".......##...............",
"........................"};

static const char* const image1_data[] = { 
"24 16 4 1",
". c None",
"a c #008080",
"b c #808080",
"# c #ff0000",
"........................",
"........................",
"...####..........####...",
"...#####........#####...",
"....######.....#####a...",
".....######...#####ba...",
".....aa###########bba...",
"....aaaaa#######aabba...",
"...aaabbb######bbbbba...",
"..aaaabb########bbbba...",
"...aaa############bba...",
"....a######aaa#####aa...",
"....######.....######...",
"...#####a........####...",
"...####aa.........###...",
"........................"};

static const char* const image2_data[] = { 
"17 15 4 1",
"# c #000000",
". c #dcdcdc",
"a c #ffff00",
"b c #ffffff",
".................",
"............#....",
".................",
"............#....",
".........#.....#.",
"....####..#.#.#..",
"...#abab#..#.#...",
"..##########..#.#",
"..#babababab##...",
"..#ababababa#.#..",
"..#babababab#..#.",
"..#ababababa#....",
"..#babababab#....",
"..#ababababa#....",
"..###########...."};


/* 
 *  Constructs a ScQtFileDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ScQtFileDlg::ScQtFileDlg( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    QPixmap image2( ( const char** ) image2_data );
    if ( !name )
	setName( "ScQtFileDlg" );
    resize( 196, 279 ); 
    if (!name)
       setCaption( tr( "FileDlg" ) );
    else
       setCaption(name);
    ScQtFileDlgLayout = new QVBoxLayout( this ); 
    ScQtFileDlgLayout->setSpacing( 6 );
    ScQtFileDlgLayout->setMargin( 11 );

    Layout5 = new QVBoxLayout; 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 0 );

    Layout4 = new QVBoxLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    Layout3 = new QHBoxLayout; 
    Layout3->setSpacing( 6 );
    Layout3->setMargin( 0 );

    TypeComboBox = new QComboBox( FALSE, this, "TypeComboBox" );
    TypeComboBox->setEditable( TRUE );
    TypeComboBox->setInsertionPolicy( QComboBox::AtBottom );
    TypeComboBox->setAutoCompletion( FALSE );
    TypeComboBox->setDuplicatesEnabled( FALSE );
    QToolTip::add(  TypeComboBox, tr( "file type filter" ) );
    QWhatsThis::add(  TypeComboBox, tr( "ComboBox FileTypeFilter\n"
"\n"
"edit or select the filter" ) );
    Layout3->addWidget( TypeComboBox );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer );

    OkButton = new QToolButton( this, "OkButton" );
    OkButton->setText( tr( "" ) );
    OkButton->setPixmap( image0 );
    OkButton->setUsesBigPixmap( FALSE );
    QToolTip::add(  OkButton, tr( "confirms the selection and closes the form" ) );
    QWhatsThis::add(  OkButton, tr( "OKButton" ) );
    Layout3->addWidget( OkButton );

    CancelButton = new QToolButton( this, "CancelButton" );
    CancelButton->setText( tr( "" ) );
    CancelButton->setPixmap( image1 );
    CancelButton->setUsesBigPixmap( FALSE );
    QToolTip::add(  CancelButton, tr( "cancels the selection and closes the form" ) );
    QWhatsThis::add(  CancelButton, tr( "CancelButton" ) );
    Layout3->addWidget( CancelButton );
    Layout4->addLayout( Layout3 );

    Layout3_2 = new QHBoxLayout; 
    Layout3_2->setSpacing( 6 );
    Layout3_2->setMargin( 0 );

    FNameLineEdit = new QLineEdit( this, "FNameLineEdit" );
    QToolTip::add(  FNameLineEdit, tr( "shows the selected filename" ) );
    QWhatsThis::add(  FNameLineEdit, tr( "Filename LineEdit\n"
"\n"
"shows the selected file\n"
"and allows the direct filename\n"
"edit" ) );
    Layout3_2->addWidget( FNameLineEdit );

    MkDirButton = new QToolButton( this, "MkDirButton" );
    MkDirButton->setText( tr( "" ) );
    MkDirButton->setPixmap( image2 );
    MkDirButton->setUsesBigPixmap( FALSE );
    QToolTip::add(  MkDirButton, tr( "confirms the selection and closes the form" ) );
    QWhatsThis::add(  MkDirButton, tr( "OKButton" ) );
    Layout3_2->addWidget( MkDirButton );
    Layout4->addLayout( Layout3_2 );

    DirComboBox = new QComboBox( FALSE, this, "DirComboBox" );
    DirComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, DirComboBox->sizePolicy().hasHeightForWidth() ) );
    DirComboBox->setEditable( TRUE );
    DirComboBox->setDuplicatesEnabled( FALSE );
    QWhatsThis::add(  DirComboBox, tr( "ComboBox Directory \n"
"edit or select the directories name" ) );
    Layout4->addWidget( DirComboBox );
    Layout5->addLayout( Layout4 );

    ListView = new QListView( this, "ListView" );
    ListView->addColumn( tr( "Name" ) );
    ListView->addColumn( tr( "size" ) );
    ListView->addColumn( tr( "type" ) );
    ListView->setRootIsDecorated( TRUE );
    QToolTip::add(  ListView, tr( "directory listview" ) );
    QWhatsThis::add(  ListView, tr( "Directory ListView\n"
"\n"
"shows the list of dirs and files" ) );
    Layout5->addWidget( ListView );
    ScQtFileDlgLayout->addLayout( Layout5 );

    // signals and slots connections
    connect( OkButton, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    connect( DirComboBox, SIGNAL( activated(int) ), this, SLOT( slotDirComboBoxChanged( int ) ) );
    connect( TypeComboBox, SIGNAL( activated(int) ), this, SLOT( slotTypeComboBoxChanged( int ) ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
    connect( ListView, SIGNAL( returnPressed(QListViewItem*) ), this, SLOT( slotSelectionChanged(QListViewItem *) ) );
    connect( ListView, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( slotSelectionChanged(QListViewItem *) ) );
    connect( ListView, SIGNAL( doubleClicked(QListViewItem*) ), this, SLOT( slotDoubleClicked(QListViewItem *) ) );
    connect( FNameLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( slotFileTextChanged( const QString & ) ) );
    connect( FNameLineEdit, SIGNAL( returnPressed() ), this, SLOT( slotOK() ) );
    connect( MkDirButton, SIGNAL( clicked() ), this, SLOT( slotMkDir() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ScQtFileDlg::~ScQtFileDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void ScQtFileDlg::slotCancel()
{
    qWarning( "ScQtFileDlg::slotCancel(): Not implemented yet!" );
}

void ScQtFileDlg::slotDirComboBoxChanged( int )
{
    qWarning( "ScQtFileDlg::slotDirComboBoxChanged( int ): Not implemented yet!" );
}

void ScQtFileDlg::slotDoubleClicked(QListViewItem *)
{
    qWarning( "ScQtFileDlg::slotDoubleClicked(QListViewItem *): Not implemented yet!" );
}

void ScQtFileDlg::slotFileTextChanged( const QString & )
{
    qWarning( "ScQtFileDlg::slotFileTextChanged( const QString & ): Not implemented yet!" );
}

void ScQtFileDlg::slotMkDir()
{
    qWarning( "ScQtFileDlg::slotMkDir(): Not implemented yet!" );
}

void ScQtFileDlg::slotOK()
{
    qWarning( "ScQtFileDlg::slotOK(): Not implemented yet!" );
}

void ScQtFileDlg::slotSelectionChanged(QListViewItem *)
{
    qWarning( "ScQtFileDlg::slotSelectionChanged(QListViewItem *): Not implemented yet!" );
}

void ScQtFileDlg::slotTypeComboBoxChanged( int )
{
    qWarning( "ScQtFileDlg::slotTypeComboBoxChanged( int ): Not implemented yet!" );
}

