#include <qhbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qlineedit.h>

#include <qpe/config.h>

#include "stringmanager.h"

using namespace Datebook;

StringManager::StringManager( const QString& str )
    : m_base( str ) {
}
StringManager::~StringManager() {

}
void StringManager::add( const QString& str ) {
    ManagerTemplate<QString>::add(str, str);
}
bool StringManager::load() {
    Config qpe( "datebook-"+m_base );
    qpe.setGroup(m_base );
    QStringList list =  qpe.readListEntry( "Names",  0x1f );
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it )
        add( (*it) );

    return true;
}
bool StringManager::save() {
    Config qpe( "datebook-"+m_base );
    qpe.setGroup(m_base);
    qpe.writeEntry( "Names", names(), 0x1f );

    return false;
}
QString StringManager::baseName()const {
    return m_base;
}



StringManagerDialog::StringManagerDialog(const StringManager& man)
    : QDialog(0, 0, true ) {
    m_base = man.baseName();

    QVBoxLayout* lay = new QVBoxLayout( this );

    m_view = new QListView( this );
    m_view->addColumn( QString::null );
    lay->addWidget( m_view );

    QHBox* box = new QHBox( this );

    QPushButton* b = new QPushButton( box );
    b->setText( tr("&Add") );
    connect(b, SIGNAL(clicked() ), this, SLOT(slotAdd() ) );

    b = new QPushButton( box );
    b->setText( tr("&Remove") );
    connect(b, SIGNAL(clicked() ), this, SLOT(slotRemove() ) );

    b = new QPushButton( box );
    b->setText( tr("Rename") );
    connect(b, SIGNAL(clicked() ), this, SLOT(slotRename() ) );

    lay->addWidget( box );

    init( man );
}
StringManagerDialog::~StringManagerDialog() {
}
StringManager StringManagerDialog::manager()const {
    StringManager man(m_base );
    QListViewItemIterator it(m_view);
    while ( it.current() ) {
        man.add( it.current()->text(0) );
        ++it;
    }

    return man;
}
void StringManagerDialog::init( const StringManager& _man ) {
    QStringList::Iterator it;
    QStringList man = _man.names();
    for ( it = man.begin(); it != man.end(); ++it )
        (void)new QListViewItem( m_view, (*it) );

}

namespace {
    class  InputDialog : public QDialog{
    public:
        InputDialog( const QString& text );
        ~InputDialog();

        QString text()const;
    private:
        QLineEdit* m_lneEdit;
    };
    InputDialog::InputDialog(const QString& text )
        : QDialog(0, 0, true ) {
        m_lneEdit = new QLineEdit( this );
        m_lneEdit->setText( text );
    }
    InputDialog::~InputDialog() {
    }
    QString InputDialog::text() const{
        return m_lneEdit->text();
    }

}

void StringManagerDialog::slotAdd() {
    InputDialog dlg(QString::null);
    dlg.setCaption( tr("Add") );
    if ( dlg.exec() == QDialog::Accepted )
        (void)new QListViewItem( m_view, dlg.text() );

}
void StringManagerDialog::slotRename() {
    QListViewItem* item = m_view->currentItem();
    if (!item) return;

    InputDialog dlg(item->text(0) );
    dlg.setCaption( tr("Rename") );

    if ( dlg.exec() == QDialog::Accepted )
        item->setText( 0, dlg.text() );
}
void StringManagerDialog::slotRemove() {
    QListViewItem* item = m_view->currentItem();
    if (!item) return;

    m_view->takeItem( item );
    delete item;
}
