#include "slfileselector.h"

SlFileIconView::SlFileIconView( QWidget* parent, const char* name )
               :QIconView( parent, name )
{
}

SlFileIconView::~SlFileIconView()
{
}

/*================================================================*/

SlFileListView::SlFileListView( QWidget* parent, const char* name )
               :QListView( parent, name )
{
}

SlFileListView::~SlFileListView()
{
}

/*================================================================*/

SlFileSelector::SlFileSelector( const QString &dirPath, const QString &mimefilter, QWidget *parent, const char *name )
               :OFileSelector( dirPath, parent, name )
{
}

SlFileSelector::~SlFileSelector()
{
}

bool SlFileSelector::isTopDir()
{
    return false;
}

void SlFileSelector::setSelected()
{
}

void SlFileSelector::setSelected(const QString&)
{
    int a;
}

void SlFileSelector::createFileList()
{
    int a;
}

void SlFileSelector::createFileList(QDir&)
{
    int a;
}

const QFileInfo* SlFileSelector::selected()
{
    int a;
}

void SlFileSelector::setCurrentFile(const QString&)
{
    int a;
}

const QString SlFileSelector::currentDir()
{
    int a;
}


void SlFileSelector::focusInEvent(QFocusEvent*)
{
}

void SlFileSelector::show()
{
}

void SlFileSelector::keyPressEvent( QKeyEvent *e )
{
}


