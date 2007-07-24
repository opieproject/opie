#include <qdatetime.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <qpe/config.h>

#include <opie/odatebookaccess.h>
#include <opie/odatebookaccessbackend_xml.h>

#include "editor.h"
#include "templatemanager.h"

using namespace Datebook;


TemplateManager::TemplateManager() {

}
TemplateManager::~TemplateManager() {
}
bool TemplateManager::save() {
    QStringList lst = names();
    Config conf( "datebook-templates");
    conf.setGroup( "___Names___");
    conf.writeEntry( "Names", names(), 0x1f );

    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        conf.setGroup( (*it) );
        conf.writeEntry( "Uid", value( (*it) ).uid() );
    }

    return true;
}
bool TemplateManager::load() {
    Config conf( "datebook-templates");
    conf.setGroup( "___Names___");
    QStringList lst = conf.readListEntry( "Names", 0x1f );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        conf.setGroup( (*it) );
        add( (*it),  OEvent() );
    }
    return true;
}

namespace {
    class TemplateItem : public QListViewItem {
    public:
        TemplateItem( QListView*, const QString& text,
                      const OEvent& );
        ~TemplateItem();

        QString text()const;
        OEvent event()const;

        void setText( const QString& text );
        void setEvent( const OEvent& );
    private:
        QString m_text;
        OEvent m_event;
    };
    TemplateItem::TemplateItem( QListView* view,
                                const QString& text,
                                const OEvent& ev )
        : QListViewItem( view ), m_event(ev) {
        QListViewItem::setText( 0, text );
    }
    TemplateItem::~TemplateItem() {
    }
    void TemplateItem::setText( const QString& text ) {
        QListViewItem::setText( 0, text );
        m_text = text;
    }
    void TemplateItem::setEvent( const OEvent& ev ) {
        m_event = ev;
    }
    QString TemplateItem::text()const {
        return m_text;
    }
    OEvent TemplateItem::event()const {

    }

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

TemplateDialog::TemplateDialog( const TemplateManager& man, Editor* edit )
    : QDialog(0, 0, true ) {
    QVBoxLayout* lay = new QVBoxLayout(this);
    m_view = new QListView( this );
    m_view->addColumn( tr("Template Names") );
    lay->addWidget( m_view );

    QHBox* box = new QHBox( this );
    lay->addWidget( box );

    QPushButton* b = new QPushButton( box );
    b->setText(tr("&Add") );
    connect( b, SIGNAL(clicked() ), this, SLOT(slotAdd() ) );

    b = new QPushButton( box );
    b->setText(tr("&Edit") );
    connect( b, SIGNAL(clicked() ), this, SLOT(slotEdit() ) );

    b = new QPushButton( box );
    b->setText(tr("&Rename") );
    connect(b, SIGNAL(clicked() ), this, SLOT(slotRename() ) );

    b = new QPushButton( box );
    b->setText(tr("Re&move") );
    connect(b, SIGNAL(clicked() ), this, SLOT(slotRemove() ) );

    init( man );
    m_edit = edit;
}
TemplateDialog::~TemplateDialog() {
}
void TemplateDialog::slotAdd() {
    if ( m_edit->newEvent( QDate::currentDate() ) ) {
        (void)new TemplateItem( m_view, tr("New Template"), m_edit->event() );
    }
}
void TemplateDialog::slotEdit() {
    TemplateItem* item = static_cast<TemplateItem*>(  m_view->currentItem() );
    if (!item) return;

    if (m_edit->edit( item->event() ) )
        item->setEvent( m_edit->event() );

}
void TemplateDialog::slotRemove() {
    QListViewItem* item = m_view->currentItem();
    if (!item) return;

    m_view->takeItem( item );
    delete item;
}
void TemplateDialog::slotRename() {
    TemplateItem* item = static_cast<TemplateItem*>( m_view->currentItem() );
    if (!item) return;

    InputDialog dlg( item->text() );
    dlg.setCaption( tr("Rename") );
    if ( dlg.exec() == QDialog::Accepted )
        item->setText( dlg.text() );

}
TemplateManager TemplateDialog::manager()const {
    TemplateManager manager;

    QListViewItemIterator it(m_view);
    while ( it.current() ) {
        TemplateItem* item = static_cast<TemplateItem*>( it.current() );
        manager.add( item->text(), item->event() );
        ++it;
    }
    return manager;
}
void TemplateDialog::init( const TemplateManager& man ) {
    QStringList list = man.names();
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        (void)new TemplateItem( m_view, (*it), man.value( (*it) ) );
    }
}
