/****************************************************************************

Derived from a file browser which was

** copyright 2001 ljp ljp@llornkcor.com

Extensive modification by Tim Wentford to allow it to work in rotated mode

****************************************************************************/
#include "fileBrowser.h"

#include "QtrListView.h"
#include <qlineedit.h>
#include <qpushbutton.h>
#ifndef _WINDOWS
#include <unistd.h>
#endif
#include <qlayout.h>
#ifdef _WINDOWS
#include <direct.h>
#endif

#include <qpe/qpeapplication.h>


fileBrowser::fileBrowser( bool allownew, QWidget* parent,  const char* name, bool modal, WFlags fl , const QString filter, const QString iPath )
    : QDialog( parent, name, true,
	       fl/* | WStyle_Customize | WStyle_Tool*/),
      filterspec(QDir::All)
{
//    showMaximized();
    if ( !name )
	setName( "fileBrowser" );
/*
    if (parent != NULL)
    {
#ifdef OPIE
	move(0,0);
	resize( parent->width(), parent->height() );
#else
	setGeometry(parent->x(), parent->y(), parent->width(), parent->height() );
#endif
    }
*/
//    showFullScreen();
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
    dirLabel->setAlignment(AlignLeft | AlignVCenter | ExpandTabs | WordBreak);
    dirLabel->setText(currentDir.canonicalPath());

    ListView = new QtrListView( this, "ListView" );
    ListView->addColumn( tr( "Name" ) );
    ListView->setSorting( 2, FALSE);
    ListView->addColumn( tr( "Size" ) );
    ListView->setSelectionMode(QListView::Single);
    ListView->setAllColumnsShowFocus( TRUE );
    ListView->setColumnWidthMode(0, QListView::Manual);
    ListView->setColumnWidthMode(1, QListView::Manual);

    // signals and slots connections
    connect( buttonShowHidden, SIGNAL( toggled(bool) ), this, SLOT( setHidden(bool) ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( OnRoot() ) );
    connect( ListView, SIGNAL(doubleClicked(QListViewItem*)), SLOT(listDoubleClicked(QListViewItem*)) );
    connect( ListView, SIGNAL(clicked(QListViewItem*)), SLOT(listClicked(QListViewItem*)) );
    connect( ListView, SIGNAL(OnOKButton(QListViewItem*)), SLOT(listClicked(QListViewItem*)) );
    connect( ListView, SIGNAL(OnCentreButton(QListViewItem*)), SLOT(listClicked(QListViewItem*)) );
    connect( ListView, SIGNAL(OnCancelButton()), SLOT(OnCancel()) );

    QVBoxLayout* grid = new QVBoxLayout(this);
    QHBoxLayout* hgrid = new QHBoxLayout(grid);
    hgrid->addWidget(dirLabel,1);
    hgrid->addWidget(buttonShowHidden);
    hgrid->addWidget(buttonOk);
    grid->addWidget(ListView,1);
    if (allownew)
    {
	m_filename = new QLineEdit(this);
	grid->addWidget(m_filename);
	connect( m_filename, SIGNAL( returnPressed() ), this, SLOT( onReturn() ));
    }
    else
    {
	m_filename = NULL;
    }

    if (QFileInfo(iPath).exists())
    {
	currentDir.setPath(iPath);
#ifdef _WINDOWS
	_chdir(iPath.latin1());
#else
	chdir(iPath.latin1());
#endif
    }
    else
    {
	currentDir.setPath(QDir::currentDirPath());
	chdir(QDir::currentDirPath().latin1());
    }

    populateList();

    if (modal)
        QPEApplication::showDialog( this );
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
////odebug << currentDir.canonicalPath() << oendl; 
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
////        odebug << "Not a dir: "+currentDir.canonicalPath()+fileL << oendl; 
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
////    odebug << currentDir.canonicalPath() << oendl; 
}

void fileBrowser::listClicked(QListViewItem *selectedItem)
{
    if (selectedItem == NULL) return;
    QString strItem=selectedItem->text(0);

////    odebug << "" << strItem << "" << oendl; 
    

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
    {
	QListViewItem *selectedItem = ListView->selectedItem();
	if (selectedItem == NULL)
	{
	    filename = "";
	}
	else
	{
	    filename = QDir::cleanDirPath(currentDir.canonicalPath()+"/"+selectedItem->text(0));
	}
        OnOK();
    }
    chdir(strItem.latin1());
//

}

// you may want to switch these 2 functions. I like single clicks
void fileBrowser::listDoubleClicked(QListViewItem *selectedItem)
{
}

QString fileBrowser::getCurrentFile()
{
    return filename;
}

void fileBrowser::OnOK()
{
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

void fileBrowser::onReturn()
{
    QListViewItem *selectedItem = ListView->selectedItem();
    if (selectedItem == NULL)
    {
	filename = m_filename->text();
    }
    else
    {
	filename = QDir::cleanDirPath(currentDir.canonicalPath()+"/"+m_filename->text());
    }
    OnOK();
}
