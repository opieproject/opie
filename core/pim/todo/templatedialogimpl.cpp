#include <qlistview.h>
#include <qlineedit.h>

#include "mainwindow.h"
#include "todoeditor.h"
#include "todotemplatemanager.h"
#include "templatedialogimpl.h"


using namespace Todo;

namespace {
    class TemplateListItem : public QListViewItem {
    public:
        TemplateListItem( QListView*,
                          const QString& name,
                          const OTodo& );
        ~TemplateListItem();

        OTodo event()const;
        QString text()const;
        void setText(const QString& str );
        void setEvent( const OTodo& );
    private:
        QString m_name;
        OTodo m_ev;
    };

    /* implementation */
    TemplateListItem::TemplateListItem( QListView* view,
                                        const QString& text,
                                        const OTodo& ev )
        : QListViewItem( view ), m_name( text ), m_ev( ev )
    {
        QListViewItem::setText(0, m_name );
    }
    TemplateListItem::~TemplateListItem() {}
    OTodo TemplateListItem::event() const {
        return m_ev;
    }
    QString TemplateListItem::text()const {
        return m_name;
    }
    void TemplateListItem::setText( const QString& str ) {
        QListViewItem::setText(0, str );
        m_name = str;
    }
    void TemplateListItem::setEvent( const OTodo& ev) {
        m_ev = ev;
    }
}

TemplateDialogImpl::TemplateDialogImpl( MainWindow* win,
                                        TemplateManager* man )
    : TemplateDialog( win ), m_win( win), m_man( man )
{
    /* fill the listview */
    /* not the fastest way.... */
    QStringList list = man->templates();
    for (QStringList::Iterator it = list.begin();
         it != list.end(); ++it  ) {
        new TemplateListItem( listView(), (*it), man->templateEvent( (*it) ) );
    }
    listView()->addColumn( tr("Name") );

    connect( listView(), SIGNAL(clicked(QListViewItem*) ),
             this, SLOT(slotClicked(QListViewItem*) ) );
}
TemplateDialogImpl::~TemplateDialogImpl() {

}
void TemplateDialogImpl::slotAdd() {
    QString str = tr("New Template %1").arg( listView()->childCount() );
    OTodo ev;
    m_man->addEvent(str, ev);
    new TemplateListItem( listView(), str, ev );
}
void TemplateDialogImpl::slotRemove() {
    TemplateListItem* item = (TemplateListItem*) listView()->currentItem();
    listView()->takeItem( item );

    m_man->removeEvent( item->text() );

    delete item;
}
void TemplateDialogImpl::slotEdit() {
    TemplateListItem* item = (TemplateListItem*)listView()->currentItem();
    OTodo ev = m_win->currentEditor()->edit( m_win, item->event() );
    if ( m_win->currentEditor()->accepted() ) {
        qWarning("accepted");
        item->setEvent( ev );
        qWarning("Priority %d", ev.priority() );
        m_man->removeEvent( item->text() );
        m_man->addEvent( item->text(), ev );
    }
}
/*
 * we need to update
 * the text
 */

void TemplateDialogImpl::slotReturn() {
    TemplateListItem* tbl = (TemplateListItem*)listView()->currentItem();

    if (tbl->text() != edit()->text() ) {
        m_man->removeEvent( tbl->text() );
        tbl->setText( edit()->text() );
        m_man->addEvent( tbl->text(), tbl->event() );
    }
}
/* update the lineedit when changing */
void TemplateDialogImpl::slotClicked( QListViewItem* item) {
    if (!item)
        return;

    TemplateListItem* tbl = (TemplateListItem*)item;
    edit()->setText( tbl->text() );
}

