#include <qdatetime.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmultilineedit.h>
#include <qscrollview.h>

#include <opie/orecurrancewidget.h>

#include "taskeditoroverviewimpl.h"
#include "taskeditoradvancedimpl.h"
#include "taskeditoralarms.h"

#include "otaskeditor.h"

OTaskEditor::OTaskEditor(int cur)
    : QDialog(0, 0, TRUE ) {
    init();
    init( cur );
}
OTaskEditor::OTaskEditor( const OTodo& to)
    : QDialog(0, 0, TRUE ) {
    init();
    init( to );
}
OTaskEditor::~OTaskEditor() {

}
void OTaskEditor::init( int cur ) {
    OTodo to;
    if ( cur != 0 )
        to.setCategories( cur );
    load(to);
    m_uid = 1; // generate a new one
}
void OTaskEditor::init( const OTodo& to ) {
    load( to );
    m_uid = to.uid();
}
OTodo OTaskEditor::todo()const{
    qWarning("saving!");
    OTodo to;
    to.setUid(m_uid );
    m_overView->save( to );
    m_adv->save( to );
    to.setRecurrence( m_rec->recurrence() );

    return to;
}
void OTaskEditor::load(const OTodo& to) {
    m_overView->load( to );
    m_adv->load( to );
    m_rec->setRecurrence( to.recurrence(), to.hasDueDate() ? to.dueDate() : QDate::currentDate() );
}
void OTaskEditor::init() {
    QVBoxLayout* lay = new QVBoxLayout(this );
    QScrollView* view = new QScrollView( this );
    view->setResizePolicy( QScrollView::AutoOneFit );
    lay->addWidget( view );

    setCaption("Task Editor");
    QWidget* container = new QWidget( view->viewport() );
    view->addChild( container );

    QVBoxLayout* layo = new QVBoxLayout( container );
    m_tab = new OTabWidget(container );
    layo->addWidget( m_tab );
    /*
     * Add the Widgets
     */
    m_overView = new TaskEditorOverViewImpl(m_tab );
    m_tab->addTab( m_overView, QString::null, tr("Overview") );

    m_adv = new TaskEditorAdvancedImpl( m_tab );
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


    /* signal and slots */
    connect(m_overView, SIGNAL(recurranceEnabled(bool) ),
            m_rec, SLOT(setEnabled(bool) ) );

    /* connect due date changed to the recurrence tab */
    connect(m_overView, SIGNAL(dueDateChanged(const QDate&) ),
            m_rec, SLOT(setStartDate(const QDate& ) ) );


    m_tab->setCurrentTab( m_overView );
}
