#include "osplitter_example.h"

/* OPIE */

#include <opie2/osplitter.h>
#include <qpe/qpeapplication.h>
#include <opie2/oapplicationfactory.h>

/* QT*/
#include <qdir.h>
#include <qlayout.h>

using namespace Opie;

OPIE_EXPORT_APP( OApplicationFactory<OSplitterExample> )

OSplitterExample::OSplitterExample( QWidget *w,const char* n,WFlags f )
    : QWidget( w, n, f ){
    QVBoxLayout * lay = new QVBoxLayout(this);
    OSplitter * splitter = new OSplitter( Horizontal, this );
    lay->addWidget( splitter );
    
    OFileSelector *selector = new OFileSelector( splitter, OFileSelector::FileSelector,
						OFileSelector::Normal, QDir::homeDirPath(),
						QString::null );
    splitter->addWidget( selector, "zoom", tr("Selector 1") );
    
    selector = new OFileSelector( splitter, OFileSelector::FileSelector, OFileSelector::Normal,
				QDir::homeDirPath(), QString::null );
    splitter->addWidget( selector, "zoom", tr("Selector 2") );
    
}
