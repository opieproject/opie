#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpopupmenu.h>

#include <qpe/datebookmonth.h>
#include <qpe/categoryselect.h>
#include <qpe/timestring.h>

#include "taskeditoroverviewimpl.h"

/*
 * we need to hack
 */

TaskEditorOverViewImpl::TaskEditorOverViewImpl( QWidget* parent, const char* name )
    : TaskEditorOverView( parent, name ) {
    init();
}
TaskEditorOverViewImpl::~TaskEditorOverViewImpl() {
}
void TaskEditorOverViewImpl::load( const OTodo& todo) {
      /*
     * now that we're 'preloaded' we
     * need to disable the buttons
     * holding the dat
     */
    btnDue->  setEnabled( FALSE );
    btnComp-> setEnabled( FALSE );
    btnStart->setEnabled( FALSE );


    QDate date = QDate::currentDate();
    QString str = TimeString::longDateString( date );

    emit recurranceEnabled( FALSE );
    ckbStart->setChecked( FALSE );
    btnStart->setText( str );

    ckbComp->setChecked( FALSE );
    btnComp->setText( str );

    cmbProgress->setCurrentItem( todo.progress()/20 );
    cmbSum->insertItem( todo.summary(), 0 );
    cmbSum->setCurrentItem( 0 );

    ckbDue->setChecked( todo.hasDueDate() );
    btnDue->setText( TimeString::longDateString( todo.dueDate() ) );

    cmbPrio->setCurrentItem( todo.priority() -1 );
    ckbCompleted->setChecked( todo.isCompleted() );

    comboCategory->setCategories( todo.categories(), "Todo List", tr("Todo List") );

}
void TaskEditorOverViewImpl::save( OTodo& to) {
    qWarning("save it now");
    if ( ckbDue->isChecked() ) {
        to.setDueDate( m_due );
        to.setHasDueDate( true );
    }else
        to.setHasDueDate( false );
    if ( comboCategory->currentCategory() != -1 ) {
        QArray<int> arr = comboCategory->currentCategories();
        to.setCategories( arr );
    }
    to.setPriority( cmbPrio->currentItem() + 1 );
    to.setCompleted( ckbCompleted->isChecked() );
    to.setSummary( cmbSum->currentText() );
    to.setProgress( cmbProgress->currentItem() * 20 );
}
/*
 * here we will init the basic view
 * one Popup for each Date Button
 * and some other signal and slots connection
 */
void TaskEditorOverViewImpl::init() {
    QDate curDate = QDate::currentDate();
    m_start = m_comp = m_due = curDate;
    QString str = TimeString::longDateString( curDate );



    /* Start Date Picker */
    m_startPop = new QPopupMenu(this);
    m_startBook = new DateBookMonth(m_startPop, 0, TRUE );
    m_startPop->insertItem( m_startBook );
    connect( m_startBook, SIGNAL( dateClicked(int, int, int) ),
             this, SLOT(slotStartChanged(int, int, int) ) );


    /* Due Date Picker */
    m_duePop = new QPopupMenu(this);
    m_dueBook = new DateBookMonth(m_duePop, 0, TRUE );
    m_duePop->insertItem( m_dueBook );
    connect( m_dueBook, SIGNAL( dateClicked(int, int, int) ),
             this, SLOT(slotDueChanged(int, int, int) ) );

    m_compPop = new QPopupMenu(this);
    m_compBook = new DateBookMonth(m_compPop, 0, TRUE );
    m_compPop->insertItem(m_compBook );
    connect( m_compBook, SIGNAL(dateClicked(int, int, int) ),
             this, SLOT(slotCompletedChanged(int, int, int) ) );


    /*
     * another part of the hack
     * it's deprecated in Qt2 but
     * still available in my qt-copy of Qt3.1beta2
     */
    btnDue->setIsMenuButton( TRUE );
    btnStart->setIsMenuButton( TRUE );
    btnComp->setIsMenuButton( TRUE );

    /* now connect the hack */
    connect(btnDue, SIGNAL(clicked() ),
            this, SLOT(hackySlotHack2() ) );
    connect(btnStart, SIGNAL(clicked() ),
            this, SLOT(hackySlotHack1() ) );
    connect(btnComp, SIGNAL(clicked() ),
            this, SLOT(hackySlotHack3() ) );

    /* recurrance */
    connect(CheckBox7, SIGNAL(clicked() ),
            this, SLOT(slotRecClicked() ) );
}

void TaskEditorOverViewImpl::slotStartChecked() {
    qWarning("slotStartChecked");
    btnStart->setEnabled( ckbStart->isChecked() );
}
void TaskEditorOverViewImpl::slotStartChanged(int y, int m, int d) {
    m_start.setYMD( y, m, d );
    btnStart->setText( TimeString::longDateString( m_start ) );
}
void TaskEditorOverViewImpl::slotDueChecked() {
    btnDue->setEnabled( ckbDue->isChecked() );
    qWarning("slotDueChecked");
}
void TaskEditorOverViewImpl::slotDueChanged(int y, int m, int d ) {
    m_due.setYMD(y, m, d );
    btnDue->setText( TimeString::longDateString( m_due ) );
}
void TaskEditorOverViewImpl::slotCompletedChecked() {
    btnComp->setEnabled( ckbComp->isChecked() );
    qWarning("slotCompletedChecked");
}
void TaskEditorOverViewImpl::slotCompletedChanged(int y, int m, int d) {
    m_comp.setYMD( y, m, d );
    btnComp->setText( TimeString::longDateString( m_comp ) );
}
/*
 * called by a button pressed event...
 * three slots to avoid ugly name() tests
 * to sender()
 */
void TaskEditorOverViewImpl::hackySlotHack1() {
    btnStart->setDown( FALSE );
    popup( btnStart, m_startPop );
}
void TaskEditorOverViewImpl::hackySlotHack2() {
    btnDue->setDown( FALSE );
    popup( btnDue, m_duePop );
}
void TaskEditorOverViewImpl::hackySlotHack3() {
    btnComp->setDown( FALSE );
    popup( btnComp, m_compPop );
}
void TaskEditorOverViewImpl::slotRecClicked() {
    qWarning("enabled recurrance");
    emit recurranceEnabled( CheckBox7->isChecked() );
}
/*
 * GPL from TT QPushButton code
 */
void TaskEditorOverViewImpl::popup( QPushButton* pu,  QPopupMenu* pop) {
    if ( pu->mapToGlobal( QPoint(0, pu->rect().bottom() ) ).y()  + pop->sizeHint().height() <= qApp->desktop()->height() )
        pop->exec( pu->mapToGlobal( pu->rect().bottomLeft()  ) );
    else
        pop->exec( pu->mapToGlobal( pu->rect().topLeft() - QPoint(0, pu->sizeHint().height()  ) ) );
}
