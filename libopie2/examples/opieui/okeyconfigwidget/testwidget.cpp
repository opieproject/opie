#include "testwidget.h"

#include <opie2/okeyconfigwidget.h>

#include <opie2/oapplicationfactory.h>
#include <opie2/otabwidget.h>

#include <qpe/qpeapplication.h>

#include <qevent.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmainwindow.h>
#include <qdialog.h>


/**
 * QObject with signals and slots inside a .cpp
 * requires the .moc at the bottom! and a run of qmake
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    static QString appName() {
        return QString::fromLatin1("keyconfig");
    }
    MainWindow( QWidget*, const char*, WFlags fl );
    ~MainWindow() {}
private slots:
    void slotClicked();
private:
    Opie::Core::OKeyConfigManager *m_manager;
};


OPIE_EXPORT_APP(  Opie::Core::OApplicationFactory<MainWindow> )


MainWindow::MainWindow( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    QVBoxLayout *lay = new QVBoxLayout( this );
    QPushButton *btn = new QPushButton( tr("Configure" ), this );
    TestMainWindow *wid = new TestMainWindow( this, "name", 0 );

    lay->addWidget( btn );
    lay->addWidget( wid );
    m_manager = wid->manager();

    connect( btn, SIGNAL(clicked()), this, SLOT(slotClicked()) );
}

void MainWindow::slotClicked() {
    QDialog diag( this, "name", true );
    diag.setCaption( tr( "Manage Keys" ) );

    QHBoxLayout *lay = new QHBoxLayout( &diag );
    Opie::Ui::OKeyConfigWidget *wid = new Opie::Ui::OKeyConfigWidget( &diag, "key config" );
    wid->setChangeMode( Opie::Ui::OKeyConfigWidget::Queue );
    wid->insert( tr("MainWindow Options" ), m_manager );
    wid->load();

    lay->addWidget( wid );

    if ( QPEApplication::execDialog( &diag ) == QDialog::Accepted ) {
        wid->save();
    }
}

TestMainWindow::TestMainWindow( QWidget* parent, const char* slot, WFlags fl )
    : Opie::Ui::OListView( parent, slot, fl)
{
    addColumn( tr( "A Doo" ) );
    addColumn( tr( "B Doo" ) );

    m_config  = new Opie::Core::OConfig( "test_config_foo" );

    /* generate the black list */
    Opie::Core::OKeyPair::List blackList;
    blackList.append( Opie::Core::OKeyPair::leftArrowKey() );
    blackList.append( Opie::Core::OKeyPair::rightArrowKey() );
    blackList.append( Opie::Core::OKeyPair::downArrowKey() );
    blackList.append( Opie::Core::OKeyPair::upArrowKey() );

    m_manager = new Opie::Core::OKeyConfigManager( m_config, "Key Group",
                                                 blackList, false, this, "Key Manager" );

    m_manager->addKeyConfig( Opie::Core::OKeyConfigItem( tr( "Delete Action" ), "delete_key", QPixmap(),
                                                       10, Opie::Core::OKeyPair( Qt::Key_D, Qt::ShiftButton ), this,
                                                       SLOT(slotDelete(QWidget*,QKeyEvent*)) ) );
    m_manager->addKeyConfig( Opie::Core::OKeyConfigItem( tr( "Show Action" ), "show_key", QPixmap(),
                                                       11, Opie::Core::OKeyPair( Qt::Key_S, Qt::AltButton ) ) );

    connect(m_manager, SIGNAL(actionActivated(QWidget*,QKeyEvent*,const Opie::Core::OKeyConfigItem&)),
            this, SLOT(slotAction(QWidget*, QKeyEvent*, const Opie::Core::OKeyConfigItem&)) );
// when commenting the line below out the keyPressEvent will work
    m_manager->handleWidget( this );
    m_manager->load();
}

TestMainWindow::~TestMainWindow() {
    m_manager->save();
    delete m_config;
}

Opie::Core::OKeyConfigManager* TestMainWindow::manager() {
    return m_manager;
}

/*
 * This only works if we do not handle the even with m_manager->handleWidget( this )
 * So this is only for demo purposes
 */
void TestMainWindow::keyPressEvent( QKeyEvent* ev ) {
    qWarning( "String is "+ m_manager->handleKeyEvent( ev ).text() );
    qWarning( "Id was %d %d %d %d", m_manager->handleKeyEventId( ev ),ev->key(),ev->state(), ev->ascii() );
    ev->ignore();
}

void TestMainWindow::slotDelete( QWidget* wid, QKeyEvent* ev ) {
    qWarning( "Slot Delete %d %d %d", wid, ev->key(), ev->state() );
}

void TestMainWindow::slotAction( QWidget* wid, QKeyEvent* ev, const Opie::Core::OKeyConfigItem& item) {
    qWarning( "Slot Action %d %d %d %s %d", wid, ev->key(), ev->state(), item.text().latin1(),  item.id() );
}

#include "testwidget.moc"
