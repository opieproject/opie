
#include <qstring.h>
#include <qlabel.h>
#include <qheader.h>
#include <qlayout.h>

#include <qpe/qpeapplication.h>
#include <opie/oapplicationfactory.h>

#include "../osplitter.h"

#include "osplitter_mail.h"


OPIE_EXPORT_APP( OApplicationFactory<ListViews> )

class Folder {
    int dummy;
};

// -----------------------------------------------------------------

ListViews::ListViews( QWidget* p, const char* name, WFlags fl )
    : QWidget( p, name, fl ) {
    qApp->installEventFilter( this );
    m_lstFolders.setAutoDelete( true );
    QHBoxLayout *lay = new QHBoxLayout(this);

    m_splitter = new OSplitter( Horizontal, this, "SPlitter 1" );
    lay->addWidget( m_splitter );
    connect(m_splitter, SIGNAL(sizeChange(bool,const QSize&) ),
            this, SLOT(slotSizeChange(bool,const QSize&) ) );

    m_overview = new QListView( m_splitter );
    m_overview->header()->setClickEnabled( FALSE );
    m_overview->addColumn( tr("Folder") );
    m_overview->setMaximumWidth( 200 );
    m_splitter->addWidget( m_overview, "zoom", tr("Folder Overview") );
    m_splitter->setSizeChange( 300 );

    /* OSplitter starts with the small mode */
    m_messages = 0;
    m_message = m_attach  = 0;

    splitti = new OSplitter( Vertical, m_splitter, "Splitti2" );
    splitti->setSizeChange( 300 );
    splitti->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

    QLabel *lbl = new QLabel(splitti);
    lbl->setTextFormat ( Qt::RichText );
    lbl->setText("<br><br><b>Test Test  Test</b><br><br><p>Fooooo hjhh</p>");

    m_messages = new QListView( splitti );
    m_messages->addColumn(" Messages ");

    folder1 = new QListView( splitti );
    folder1->addColumn( "Messages 2 " );

    splitti->addWidget(m_messages, "mail", tr("Mails") );
    splitti->addWidget(folder1, "folder", tr("Folder") );
    splitti->addWidget( lbl, "logo", tr("Label") );
    m_message = lbl;

    m_splitter->addWidget( splitti );

}


ListViews::~ListViews() {

}


bool ListViews::eventFilter( QObject* obj, QEvent* ev ) {
    if (!obj->isWidgetType() )
        return false;
    if ( ev->type() == QEvent::MouseButtonRelease ) {
        qWarning(" name %s, class %s", obj->name(), obj->className() );
    }

    return false;
}
