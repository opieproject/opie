#include <qaction.h> // action
#include <qmenubar.h> // menubar
#include <qtoolbar.h> // toolbar
#include <qlabel.h> // a label
#include <qpushbutton.h> // the header file for the QPushButton
#include <qlayout.h>
#include <qtimer.h> // we use it for the singleShot
#include <qdatetime.h> // for QDate
#include <qtextview.h> // a rich text widget
#include <qdialog.h>
#include <qwhatsthis.h> // for whats this

#include <qpe/qpeapplication.h> // the QPEApplication
#include <qpe/resource.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/datebookmonth.h>
#include <qpe/timestring.h>

#include <opie/oapplicationfactory.h> // a template + macro to save the main method and allow quick launching
#include <opie/otabwidget.h>
#include <opie/owait.h>

#include "simple.h"

/*
 * implementation of simple
 */

/*
 * The factory is used for quicklaunching
 * It needs a constructor ( c'tor ) with at least QWidget, const char* and WFlags as parameter and a static QString appName() matching the TARGET of the .pro
 *
 * Depending on the global quick launch setting this will create
 * either a main method or one for our component plugin system
 */

OPIE_EXPORT_APP( OApplicationFactory<MainWindow> )

MainWindow::MainWindow(QWidget *parent,  const char* name, WFlags fl )
    : QMainWindow( parent, name, fl ) {
    setCaption(tr("My MainWindow") );

    m_desktopChannel = 0;
    m_loading = 0;

    initUI();


    /*
     * Tab widget as central
     */
    m_tab = new OTabWidget(this);

    setCentralWidget( m_tab );

    m_todoView = new PIMListView(m_tab, "Todo view" );
    m_tab->addTab( m_todoView,"todo/TodoList", tr("Todos") );

    m_dateView = new PIMListView(m_tab, "Datebook view" );
    m_tab->addTab( m_dateView, "datebook/DateBook", tr("Events") );

    /* now connect the actions */
    /*
     * we connect the activated to our show
     * and on activation we will show a detailed
     * summary of the record
     */
    connect(m_fire, SIGNAL(activated() ),
            this, SLOT(slotShow() ) );

    /*
     * We will change the date
     */
    connect(m_dateAction, SIGNAL(activated() ),
            this, SLOT(slotDate() ) );

    /*
     * connect the show signal of the PIMListView
     * to a slot to show a dialog
     */
    connect(m_todoView, SIGNAL(showRecord(const OPimRecord&) ),
            this, SLOT(slotShowRecord(const OPimRecord&) ) );
    connect(m_dateView, SIGNAL(showRecord(const OPimRecord&) ),
            this, SLOT(slotShowRecord(const OPimRecord&) ) );

    /*
     * Now comes the important lines of code for this example
     * We do not directly call load but delay loading until
     * all Events are dispatches and handled.
     * SO we will load once our window is mapped to screen
     * to achieve that we use a QTimer::singleShot
     * After 10 milli seconds the timer fires and on TimerEvent
     * out slot slotLoad will be called
     * Remember this a Constructor to construct your object and not
     * to load
     */
    QTimer::singleShot( 10, this, SLOT(slotLoad() ) );
}

MainWindow::~MainWindow() {
    // again nothing to delete because Qt takes care
}


void MainWindow::setDocument( const QString& /*str*/ ) {
}


void MainWindow::initUI() {

    setToolBarsMovable( false );

    QToolBar *menuBarHolder = new QToolBar( this );

    menuBarHolder->setHorizontalStretchable( true );
    QMenuBar *mb = new QMenuBar( menuBarHolder );
    QToolBar *tb = new QToolBar( this );

    QPopupMenu *fileMenu = new QPopupMenu( this );


    QAction *a = new QAction( tr("Quit"), Resource::loadIconSet("quit_icon"),
                              QString::null, 0, this, "quit_action" );
    /*
     * Connect quit to the QApplication quit slot
     */
    connect(a, SIGNAL(activated() ),
            qApp, SLOT(quit() ) );
    a->addTo( fileMenu );

    a =  new QAction(tr("View Current"),
                     Resource::loadIconSet("zoom"),
                     QString::null, 0, this, "view current");
    /* a simple whats this online explanation of out button */
    a->setWhatsThis(tr("Views the current record")  );
    /* or QWhatsThis::add(widget, "description" ); */

    /* see  the power? */
    a->addTo( fileMenu );
    a->addTo( tb );
    m_fire = a;

    a = new QAction(tr("Choose Date"),
                    Resource::loadIconSet("day"),
                    QString::null, 0, this, "choose date" );
    a->addTo( fileMenu );
    a->addTo( tb );
    m_dateAction = a;

    mb->insertItem(tr("File"), fileMenu );

}

void MainWindow::slotLoad() {
    /*
     * There is no real shared access in the PIM API
     * It wasn't finish cause of health problems of one
     * of the authors so we do something fancy and use QCOP
     * the IPC system to get a current copy
     */
    /* NOTES to QCOP: QCOP operates over channels and use QDataStream
     * to send data. You can check if a channel isRegistered or hook
     * yourself to that channel. A Channel is QCString and normally
     * prefix with QPE/ and then the system in example QPE/System,
     * QPE/Desktop a special channel is the application channel
     * it QPE/Application/appname this channel gets created on app
     * startup by QPEApplication. QCOP is asynchronous
     *
     * To send you'll use QCopEnevelope
     *
     */
    /*
     * What we will do is first is get to know if either
     * datebook or todolist are running if so we will kindly
     * asked to save the data for us.
     * If neither are running we can load directly
     */
    if (!QCopChannel::isRegistered("QPE/Application/todolist") &&
        !QCopChannel::isRegistered("QPE/Application/datebook") ) {
        m_db.load();
        m_tb.load();
        return slotLoadForDay( QDate::currentDate() );
    }

    /*
     * prepare our answer machine the QCopChannel
     * QPE/Desktop will send "flushDone(QString)" when
     * the flush is done it emits a signal on receive
     */
    m_desktopChannel = new QCopChannel("QPE/Desktop");
    connect(m_desktopChannel, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(slotDesktopReceive(const QCString&,const QByteArray&) ) );
    /* the numberof synced channels will be set to zero */
    m_synced = 0;

    /*
     * We use {} around the QCopEnvelope because it sends its
     * data on destruction of QCopEnvelope with
     */
    /* check again  if not present increment synced*/
    if ( QCopChannel::isRegistered("QPE/Application/todolist") ) {
        QCopEnvelope env("QPE/Application/todolist", "flush()" );
        // env << data; but we do not have any parameters here
    }else
        m_synced++;

    if ( QCopChannel::isRegistered("QPE/Application/datebook") ) {
        QCopEnvelope env("QPE/Application/datebook", "flush()" );
    }else
        m_synced++;

    /* we will provide a wait scrren */
    m_loading = new OWait(this, "wait screen" );
}

void MainWindow::slotDesktopReceive(const QCString& cmd, const QByteArray& data ) {
    /* the bytearray was filled with the QDataStream now
     * we open it read only to get the value(s)
     */
    QDataStream stream(data, IO_ReadOnly );
    /*
     * we're only interested in the flushDone
     */
    if ( cmd == "flushDone(QString)" ) {
        QString appname;
        stream >> appname; // get the first argument out of stream
        if (appname == QString::fromLatin1("datebook") ||
            appname == QString::fromLatin1("todolist") )
            m_synced++;
    }

    /*
     * If we synced both we can go ahead
     * In future this is not needed anymore when we finally
     * implemented X-Ref and other PIM features
     */
    if (m_synced >= 2 ) {
        delete m_loading;
        delete m_desktopChannel;
        /* now we finally can start doing the actual loading */
        m_tb.load();
        m_db.load();
        {
            /* tell the applications to reload */
            QCopEnvelope("QPE/Application/todolist", "reload()");
            QCopEnvelope("QPE/Application/datebook", "reload()");
        }
        slotLoadForDay( QDate::currentDate() );
    }

}

/* overloaded member for shortcoming of libqpe */
void MainWindow::slotLoadForDay(int y, int m, int d) {
    /* year,month, day */
    slotLoadForDay( QDate(y, m, d ) );
}

void MainWindow::slotLoadForDay(const QDate& date) {


    /* all todos for today including the ones without dueDate */
    m_todoView->set( m_tb.effectiveToDos(date, date ) );
    m_dateView->set( m_db.effectiveEvents( date, date ) );
}

/* we want to show the current record */
void MainWindow::slotShow() {
    /* we only added PIMListViews so we can safely cast */
    PIMListView *view = static_cast<PIMListView*>(m_tab->currentWidget() );

    /* ask the view to send a signal */
    view->showCurrentRecord();

}

/* as answer this slot will be called */
void MainWindow::slotShowRecord( const OPimRecord& rec) {
    /* got a parent but still is a toplevel MODAL dialog */
    QDialog* dia = new QDialog(this,"dialog",TRUE );
    QVBoxLayout *box = new QVBoxLayout( dia );
    dia->setCaption( tr("View Record") );


    QTextView *view = new QTextView(dia );
    view->setText( rec.toRichText() );
    box->addWidget( view );
    /*
     * execute via QPEApplication
     * this allows QPEApplication to make a sane decision
     * on the size
     */
    dia->showMaximized();
    QPEApplication::execDialog( dia );
    delete dia;
}


void MainWindow::slotDate() {
    /*
     * called by the action we will show a Popup
     * at the current mouse position with a DateChooser
     * to select the day
     */
    qWarning("slot Date");
    QPopupMenu *menu = new QPopupMenu();
    /* A Month to select a date from TRUE for auto close */
    DateBookMonth *month = new DateBookMonth(menu, 0, true );
    connect(month, SIGNAL(dateClicked(int,int,int) ),
            this, SLOT(slotLoadForDay(int,int,int) ) );

    menu->insertItem( month );

    menu->exec( QCursor::pos() );

    /*
     * we do not need to delete month because
     * we delete its parent menu
     */

    delete menu;
}

/*
 * An anonymous namespace this symbol is only available here
 * so truely private
 */
namespace {
    /* not static cause namespace does that what static would do */
    const int RTTI = 5050;
    /*
     * every ListView got Items. we've special pim items
     * holding ownership and the pointer to a pim record
     * it can't hold a pimrecord directly because this
     * would introduce slicing... any break
     */
    /*
     * A struct is a special class. Everything is public by
     * default.
     */
    struct PIMListViewItem : public QListViewItem {
        /*
         *currently no hierachies are planed for the example
         * so only one constructor with a QListView as parent
         */
        PIMListViewItem( QListView*, OPimRecord* record );
        ~PIMListViewItem();

        /* used by the QListViewItem to easily allow identifiying of different
         * items. Values greater than 1000 should be used */
        int rtti()const;
        OPimRecord* record()const;

    private:
        OPimRecord* m_record;
    };

    PIMListViewItem::PIMListViewItem( QListView *p, OPimRecord* rec )
        : QListViewItem(p), m_record( rec ) {
    }

    PIMListViewItem::~PIMListViewItem() {
        /* we've the onwership so we need to delete it */
        delete m_record;
    }

    OPimRecord* PIMListViewItem::record()const {
        return m_record;
    }

}


PIMListView::PIMListView( QWidget* widget, const char* name, WFlags fl  )
    : QListView(widget, name, fl )
{
    addColumn("Summary");
}

PIMListView::~PIMListView() {

}

void PIMListView::set( OTodoAccess::List list ) {
    /* clear first and then add new items */
    clear();

    OTodoAccess::List::Iterator it;
    for (it = list.begin(); it != list.end(); ++it ) {
        /*
         * make a new item which automatically gets added to the listview
         * and call the copy c'tor to create a new OTodo
         */
        PIMListViewItem *i = new PIMListViewItem(this, new OTodo( *it ) );
        i->setText(0, (*it).summary() );
    }
}

void PIMListView::set( const OEffectiveEvent::ValueList& lst ) {
    /* clear first and then add items */
    clear();

    OEffectiveEvent::ValueList::ConstIterator it;
    for ( it = lst.begin(); it != lst.end(); ++it ) {
        PIMListViewItem *i = new PIMListViewItem(this, new OEvent( (*it).event() ) );
        i->setText( 0, PIMListView::makeString( (*it) ) );
    }

}

void PIMListView::showCurrentRecord() {
    /* it could be possible that their is no currentItem */
    if (!currentItem() )
        return;

    /*
     * we only add PIMListViewItems so it is save
     * to do this case. If this would not be the case
     * use rtti() to check in a switch() case
     */
    PIMListViewItem *item = static_cast<PIMListViewItem*>( currentItem() );

    /* finally you see how to emit a signal */
    emit showRecord( (*item->record() ) );
}

QString PIMListView::makeString( const OEffectiveEvent& ev ) {
    QString str;
    str += ev.description();
    if ( !ev.event().isAllDay() ) {
        if ( ev.startDate() != ev.endDate() ) {
            str += tr("Start ") + TimeString::timeString( ev.event().startDateTime().time() );
            str += " - " + TimeString::longDateString( ev.startDate() );
            str +=  tr("End ") + TimeString::timeString( ev.event().endDateTime().time() );
            str += " - " + TimeString::longDateString( ev.endDate() );
        }else{
            str += tr("Time ") + TimeString::timeString( ev.startTime() );
            str += " - " + TimeString::timeString( ev.endTime() );
        }
    }else
        str += tr(" This is an All-Day Event");

    return str;
}
