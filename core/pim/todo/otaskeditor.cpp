#include <qdatetime.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmultilineedit.h>

#include <opie/orecurrancewidget.h>

#include "taskeditoroverviewimpl.h"
#include "taskeditoradvanced.h"
#include "taskeditoralarms.h"

#include "otaskeditor.h"

OTaskEditor::OTaskEditor(int cur)
    : QDialog(0, 0, TRUE ) {
    init();
    OTodo to;
    to.setCategories( cur );
    load(to);
    m_uid = 1; // generate a new one
}
OTaskEditor::OTaskEditor( const OTodo& to)
    : QDialog(0, 0, TRUE ) {
    init();
    load( to );
    m_uid = to.uid();
}
OTaskEditor::~OTaskEditor() {

}
OTodo OTaskEditor::todo()const{
    qWarning("saving!");
    OTodo to;
    to.setUid(m_uid );
    m_overView->save( to );
    to.setDescription( m_line->text() );

    return to;
}
void OTaskEditor::load(const OTodo& to) {
    m_overView->load( to );
    m_line->setText( to.description() );
}
void OTaskEditor::init() {
    QVBoxLayout* lay = new QVBoxLayout(this);
    setCaption("Task Editor");
    m_tab = new OTabWidget(this);

    /*
     * Add the Widgets
     */
    m_overView = new TaskEditorOverViewImpl(m_tab );
    m_tab->addTab( m_overView, QString::null, tr("Overview") );

    m_adv = new TaskEditorAdvanced( m_tab );
    m_line = new QMultiLineEdit(m_adv );
    QLabel* label = new QLabel(m_adv );
    label->setText( tr("Description") );
    ((QGridLayout*) m_adv->layout() )->addWidget( label,3, 0 );
    ((QGridLayout*) m_adv->layout())->addWidget( m_line,4,0 );
    m_tab->addTab( m_adv, QString::null, tr("Advanced") );

    m_alarm = new TaskEditorAlarms( m_tab );
    m_tab->addTab( m_alarm, QString::null, tr("Alarms") );

    m_remind = new TaskEditorAlarms( m_tab );
    m_tab->addTab( m_remind, QString::null, tr("Reminders") );

    QLabel* lbl = new QLabel(m_tab );
    lbl->setText( tr("X-Ref") );
    m_tab->addTab( lbl, QString::null, tr("X-Ref") );

    m_rec = new ORecurranceWidget( true, QDate::currentDate(), this );
    m_tab->addTab( m_rec, QString::null, tr("Recurrance") );

    lay->addWidget(m_tab );

    /* signal and slots */
    connect(m_overView, SIGNAL(recurranceEnabled(bool) ),
            m_rec, SLOT(setEnabled(bool) ) );

    m_tab->setCurrentTab( m_overView );
}
