/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <qlineedit.h>

#include "mainwindow.h"
#include "todoeditor.h"
#include "todotemplatemanager.h"
#include "templatedialogimpl.h"


using namespace Todo;

/* TRANSLATOR Todo::TemplateDialogImpl */

namespace {
    class TemplateListItem : public QListViewItem {
    public:
        TemplateListItem( QListView*,
                          const QString& name,
                          const OPimTodo& );
        ~TemplateListItem();

        OPimTodo event()const;
        QString text()const;
        void setText(const QString& str );
        void setEvent( const OPimTodo& );
    private:
        QString m_name;
        OPimTodo m_ev;
    };

    /* implementation */
    TemplateListItem::TemplateListItem( QListView* view,
                                        const QString& text,
                                        const OPimTodo& ev )
        : QListViewItem( view ), m_name( text ), m_ev( ev )
    {
        QListViewItem::setText(0, m_name );
    }
    TemplateListItem::~TemplateListItem() {}
    OPimTodo TemplateListItem::event() const {
        return m_ev;
    }
    QString TemplateListItem::text()const {
        return m_name;
    }
    void TemplateListItem::setText( const QString& str ) {
        QListViewItem::setText(0, str );
        m_name = str;
    }
    void TemplateListItem::setEvent( const OPimTodo& ev) {
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
    listView()->addColumn( QWidget::tr("Name") );

    connect( listView(), SIGNAL(clicked(QListViewItem*) ),
             this, SLOT(slotClicked(QListViewItem*) ) );
}
TemplateDialogImpl::~TemplateDialogImpl() {

}
void TemplateDialogImpl::slotAdd() {
    QString str = QWidget::tr("New Template %1").arg( listView()->childCount() );
    OPimTodo ev;
    m_man->addEvent(str, ev);
    new TemplateListItem( listView(), str, ev );
}
void TemplateDialogImpl::slotRemove() {
    if (!listView()->currentItem() )
        return;

    TemplateListItem* item = static_cast<TemplateListItem*>(  listView()->currentItem() );
    listView()->takeItem( item );

    m_man->removeEvent( item->text() );

    delete item;
}
void TemplateDialogImpl::slotEdit() {
    if ( !listView()->currentItem() )
        return;

    TemplateListItem* item = static_cast<TemplateListItem*>( listView()->currentItem() );
    OPimTodo ev = m_win->currentEditor()->edit( m_win, item->event() );
    if ( m_win->currentEditor()->accepted() ) {
        item->setEvent( ev );
        m_man->removeEvent( item->text() );
        m_man->addEvent( item->text(), ev );
    }
}
/*
 * we need to update
 * the text
 */

void TemplateDialogImpl::slotReturn() {
    if ( !listView()->currentItem() )
        return;

    TemplateListItem* tbl = static_cast<TemplateListItem*>( listView()->currentItem() );

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

    TemplateListItem* tbl = static_cast<TemplateListItem*>(item);
    edit()->setText( tbl->text() );
}

