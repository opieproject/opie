/****************************************************************************

Derived from a file browser which was

** copyright 2001 ljp ljp@llornkcor.com

Extensive modification by Tim Wentford to allow it to work in rotated mode

****************************************************************************/
#include "fileBrowser.h"

#include "QtrListView.h"
#include <qpushbutton.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <qlayout.h>

fileBrowser::fileBrowser( QWidget* parent,  const char* name, bool modal, WFlags fl , const QString filter, const QString iPath )
    : QDialog( parent, name, modal, fl ), filterspec(QDir::All)
{
//    showMaximized();
    if ( !name )
	setName( "fileBrowser" );
    if (parent != NULL) resize( parent->width(), parent->height() );
    setCaption(tr( "Browse for file" ) );
    filterStr=filter;

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setFixedSize( 25, 25 );
    buttonOk->setAutoDefault( false );
    buttonOk->setText( tr( "/" ) );

    buttonShowHidden = new QPushButton( this, "buttonShowHidden" );
//    buttonShowHidden->setFixedSize( 50, 25 );
    buttonShowHidden->setText( tr( "Hidden" ) );
    buttonShowHidden->setAutoDefault( false );
    buttonShowHidden->setToggleButton( true );
    buttonShowHidden->setOn( false );

    dirLabel = new QLabel(this, "DirLabel");
    dirLabel->setText(currentDir.canonicalPath());

    ListView = new QtrListView( this, "ListView" );
    ListView->addColumn( tr( "Name" ) );
    ListView->setSorting( 2, FALSE);
    ListView->addColumn( tr( "Size" ) );
    ListView->setSelectionMode(QListView::Single);
    ListView->setAllColumnsShowFocus( TRUE );

    // signals and slots connections
    connect( buttonShowHidden, SIGNAL( toggled(bool) ), this, SLOT( setHidden(bool) ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( OnRoot() ) );
    connect( ListView, SIGNAL(doubleClicked( QListViewItem*)), SLOT(listDoubleClicked(QListViewItem *)) );
    connect( ListView, SIGNAL(clicked( QListViewItem*)), SLOT(listClicked(QListViewItem *)) );
    connect( ListView, SIGNAL(OnOKButton( QListViewItem*)), SLOT(listClicked(QListViewItem *)) );
    connect( ListView, SIGNAL(OnCentreButton( QListViewItem*)), SLOT(listClicked(QListViewItem *)) );
    connect( ListView, SIGNAL(OnCancelButton()), SLOT(OnCancel()) );

    QVBoxLayout* grid = new QVBoxLayout(this);
    QHBoxLayout* hgrid = new QHBoxLayout(grid);
    hgrid->addWidget(dirLabel,1);
    hgrid->addWidget(buttonShowHidden);
    hgrid->addWidget(buttonOk);
    grid->addWidget(ListView,1);

    if (QFileInfo(iPath).exists())
    {
	currentDir.setPath(iPath);
	chdir(iPath.latin1());
    }
    else
    {
	currentDir.setPath(QDir::currentDirPath());
	chdir(QDir::currentDirPath().latin1());
    }

    populateList();
}

void fileBrowser::resizeEvent(QResizeEvent* e)
{
    ListView->setColumnWidth(1,(ListView->width())/4);
    ListView->setColumnWidth(0,ListView->width()-20-ListView->columnWidth(1));    
}

fileBrowser::~fileBrowser()
{
}


void fileBrowser::populateList()
{
    ListView->clear();
//qDebug(currentDir.canonicalPath());
//    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoSymLinks );
    currentDir.setFilter( filterspec );
    currentDir.setSorting(/* QDir::Size*/ /*| QDir::Reversed | */QDir::DirsFirst);
    currentDir.setMatchAllDirs(TRUE);

    currentDir.setNameFilter(filterStr);
//    currentDir.setNameFilter("*.txt;*.etx");
    QString fileL, fileS;
    const QFileInfoList *list = currentDir.entryInfoList();
    QFileInfoListIterator it(*list);
    QFileInfo *fi;
    while ( (fi=it.current()) )
    {
	if (fi->fileName() != ".")
	{
	    fileS.sprintf( "%10li", fi->size() );
	    fileL.sprintf( "%s",fi->fileName().data() );
	    if( fi->isDir() )
	    {
		fileL+="/";
	    } 
	    else 
	    {
//        qDebug("Not a dir: "+currentDir.canonicalPath()+fileL);
	    }
	    new QListViewItem( ListView,fileL,fileS );
	}
	++it;
    }
    ListView->setSorting( 2, FALSE);
    dirLabel->setText("Current Directory:\n"+currentDir.canonicalPath());
    ListView->setFocus();
}

void fileBrowser::upDir()
{
//    qDebug(currentDir.canonicalPath());
}

void fileBrowser::listClicked(QListViewItem *selectedItem)
{
    if (selectedItem == NULL) return;
    QString strItem=selectedItem->text(0);

//    qDebug("%s", (const char*)strItem);
    

    QString strSize=selectedItem->text(1);

    strSize.stripWhiteSpace();

    bool ok;

    QFileInfo fi(strItem);
    while (fi.isSymLink()) fi.setFile(fi.readLink());
    if (fi.isDir())
    {
        strItem=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+strItem);

	if(QDir(strItem).exists())
	{
	    currentDir.cd(strItem, TRUE);
	    populateList();
	}
    } else
        OnOK();
    chdir(strItem.latin1());
//

}

// you may want to switch these 2 functions. I like single clicks
void fileBrowser::listDoubleClicked(QListViewItem *selectedItem)
{
}

void fileBrowser::OnOK() {

    QListViewItemIterator it1( ListView);
    for ( ; it1.current(); ++it1 ) {
	if ( it1.current()->isSelected() ) {
	    selectedFileName=QDir::cleanDirPath(currentDir.canonicalPath()+"/"+it1.current()->text(0));
//	    qDebug("selected filename is "+selectedFileName);
	    fileList.append( selectedFileName );
	}
    }
    accept();
}

void fileBrowser::OnRoot()
{
    currentDir.cd("/", TRUE);
    populateList();
    chdir("/");
}

void fileBrowser::OnCancel()
{
    reject();
}

void fileBrowser::setHidden(bool _hidden)
{
    if (_hidden)
	filterspec = QDir::All | QDir::Hidden;
    else
	filterspec = QDir::All;
    populateList();
}
