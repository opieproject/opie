
#include <qlayout.h>
#include <qaction.h>
#include <qdir.h>
#include <qmenubar.h>
#include <qpopupmenu.h>

#include <qpe/categories.h>

#include <opie/ocontactdb.h>
#include <opie/tododb.h>

#include "mainwidget.h"
#include "mainlistview.h"
#include "maintextview.h"
#include "mainwindow.h"

using namespace Viewer;

MainWindow::MainWindow( QWidget *parent,  const char* name )
    : QMainWindow(parent, name ), m_main(0),
      m_context(0),
      m_file(0), m_reload(0) {

    qWarning("MainWindow" );
    setCaption( "Opie PIM Viewer");
    m_field.fill(TRUE, 3);
    initGUI();
    doReload();

};
MainWindow::~MainWindow() {


}
void MainWindow::initGUI() {
    // our layout of choice


    m_main = new MainWidget(this );
    setCentralWidget( m_main);
    m_main->textView()->hide();
    connect( m_main->listView(), SIGNAL(selected(const QString&, const QString& ) ),
             m_main->textView(), SLOT( slotShow(const QString&, const QString& ) ) );

    // connect main widget slots

    QPopupMenu* men = 0;
    QAction* a = 0;

    // Edit
    men = new QPopupMenu();
    menuBar()->insertItem( tr("Edit"), men );
    a = new QAction();
    a->setText(tr("Reload") );
    a->addTo(men);
    connect(a, SIGNAL(activated() ),
            this, SLOT(reload() ) );

    a = new QAction();
    a->setText( tr("Show current") );
    a->addTo(men);
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotShowCurrent() ) );

    a = new QAction();
    a->setText( tr("Edit current") );
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotEditCurrent() ) );


    // Option menu
    men = new QPopupMenu();
    menuBar()->insertItem( tr("Options"),  men);

    a = new QAction();
    a->setText( tr("Show Addresses") );
    a->setToggleAction( true );
    a->setOn( true );
    a->addTo( men );
    connect(a, SIGNAL(toggled(bool) ),
            this, SLOT(showAddresses(bool) ) );
    m_ab = a;

    a = new QAction();
    a->setText( tr("Show Todos") );
    a->setToggleAction( true );
    a->setOn( true );
    a->addTo( men );
    connect(a, SIGNAL(toggled(bool) ),
            this, SLOT(showTodos(bool) ) );
    m_todo = a;

    a = new QAction();
    a->setText( tr("Show Dates") );
    a->setToggleAction( true );
    a->setOn( true );
    a->addTo( men );
    connect(a, SIGNAL(toggled(bool) ),
            this, SLOT(showDates(bool) ) );
    m_date =a;

    men->insertSeparator();

    a = new QAction();
    a->setText( tr("Show Informations") );
    a->setToggleAction( true );
    a->addTo( men );
    connect(a, SIGNAL(toggled(bool) ),
            this, SLOT(toggleRichText(bool) ) );

};
void MainWindow::toggleRichText(bool toggle ) {
    if (toggle) m_main->textView()->show();
    else m_main->textView()->hide();
}
void MainWindow::reload() {
    doReload();
}
void MainWindow::closeMe() {

}
void MainWindow::slotShow(const QString& app,  int id) {

}
void MainWindow::slotEdit(const QString& app, int id ) {

}
void MainWindow::slotNew( const QString& ) {

}
/* This is responsible for filling the ListView with life
 * It'll load the Category description and create toplevel
 * ListViewItems. Then it'll load datebook and fill all
 * Events into the List.
 * Then the Addressbook will be loaded and all addresses will be
 * loaded.
 * Then the todolist
 *
 */
void MainWindow::doReload() {
    m_main->listView()->clear();
    Categories cats;
    cats.load( QDir::homeDirPath() + "/Settings/Categories.xml" );
    QStringList categories = cats.labels(QString::null, true, Categories::UnfiledLabel );
    QStringList::Iterator it;
    for ( it = categories.begin(); it != categories.end(); ++it ) {
        qWarning("Cat Name %s",  (*it).latin1() );
        int id = cats.id( QString::null,  (*it) );
        m_main->listView()->insertCategory( (*it), id  );
    }
    // Contacts
    if ( m_ab->isOn() ) {
        OContactDB db("viewer");
        QValueList<Contact> contacts = db.allContacts();
        QValueList<Contact>::Iterator contactIt;
        for ( contactIt = contacts.begin(); contactIt != contacts.end(); ++contactIt ) {
            QArray<int> ints = (*contactIt).categories();
            if ( ints.size() == 0 ) {
                m_main->listView()->insertEntry( (*contactIt).uid(),
                                                 tr("Unfiled"),
                                                 0,
                                                 "Addressbook",
                                                 smallAbText( (*contactIt) ),
                                                 (*contactIt).toRichText(),
                                                 QPixmap() );

            }
            for ( uint i = 0; i < ints.size(); i++ ) {
                m_main->listView()->insertEntry( (*contactIt).uid(),
                                                 cats.label(QString::null, ints[i] ),
                                                 ints[i],
                                                 "Addressbook",
                                                 smallAbText( (*contactIt) ),
                                                 (*contactIt).toRichText(),
                                                 QPixmap() );
            }
        }
    }
    // Todos
    if (m_todo->isOn() ) {
        ToDoDB todoDB;
        QValueList<ToDoEvent> todos = todoDB.rawToDos();
        QValueList<ToDoEvent>::Iterator todoIt;
        for ( todoIt =todos.begin(); todoIt != todos.end(); ++todoIt ) {
            QArray<int> ints = (*todoIt).categories();
            if (ints.size() == 0 )
                m_main->listView()->insertEntry( (*todoIt).uid(),
                                                 tr("Unfiled"),
                                                 0,
                                                 "Todo",
                                                 smallTodoText( (*todoIt) ),
                                                 (*todoIt).richText(),
                                                 QPixmap() );
            for (uint i = 0; i < ints.size(); i++ ) {
                m_main->listView()->insertEntry( (*todoIt).uid(),
                                                 cats.label( QString::null,  ints[i]),
                                                 ints[i],
                                                 "Todo",
                                                 smallTodoText( (*todoIt) ),
                                                 (*todoIt).richText(),
                                                 QPixmap() );
            }
        }
    }
    if (m_date->isOn() ) {

    }

}
void MainWindow::showAddresses( bool show ) {
    m_field.setBit(ShowAddress, show );
    doReload();
}
void MainWindow::showTodos( bool show ) {
    m_field.setBit(ShowAddress, show );
    doReload();
}
void MainWindow::showDates(bool show ) {
    m_field.setBit(ShowAddress, show );
    doReload();
}
QString MainWindow::smallAbText( const Contact& contact ) {
    QString text;
    text += contact.fullName();
    return text;
}
QString MainWindow::smallTodoText( const ToDoEvent& todo ) {
    QString text;
    if ( !todo.summary().isEmpty() )
        text += todo.summary();
    else
        text += todo.description().left( 10 );

    text += "  " + QString::number( todo.progress() ) + "%";

    if ( todo.hasDate() )
        text += tr(" Due ") + todo.date().toString() ;
    return text;
}
