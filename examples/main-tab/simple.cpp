#include <qaction.h> // action
#include <qmenubar.h> // menubar
#include <qtoolbar.h> // toolbar
#include <qlabel.h> // a label
#include <qpushbutton.h> // the header file for the QPushButton
#include <qlayout.h>

#include <qpe/qpeapplication.h> // the QPEApplication
#include <qpe/resource.h>
#include <qpe/sound.h>

#include <opie/oapplicationfactory.h> // a template + macro to save the main method and allow quick launching
#include <opie/otabwidget.h>

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

    initUI();


    /*
     * Tab widget as central
     */
    OTabWidget *tab = new OTabWidget(this);
    connect(tab, SIGNAL(currentChanged(QWidget*) ),
            this, SLOT( slotCurrentChanged(QWidget*) ) );
    setCentralWidget( tab );

    Simple1 *simple1 = new Simple1( this );
    tab->addTab( simple1, "new",  tr("Simple1") );
    tab->setCurrentTab( tr("Simple1") );

    Simple2 *simple2 = new Simple2( this );
    tab->addTab( simple2, "trash", tr("Simple2") );

    m_oldCurrent = simple1;

    connect(m_fire, SIGNAL(activated() ),
            simple1, SLOT(slotFire() ) );
}

MainWindow::~MainWindow() {
    // again nothing to delete because Qt takes care
}


void MainWindow::setDocument( const QString& /*str*/ ) {
}
void MainWindow::slotCurrentChanged( QWidget *wid) {
    disconnect(m_fire, SIGNAL(activated() ),
               m_oldCurrent, SLOT(slotFire() ) );
    connect(m_fire, SIGNAL(activated() ),
            wid, SLOT(slotFire() ) );

    m_oldCurrent = wid;
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

   a =  new QAction(tr("Fire"),
                             Resource::loadIconSet("new"),
                             QString::null, 0, this, "fire_button");

    /* see  the power? */
    a->addTo( fileMenu );
    a->addTo( tb );
    m_fire = a;


    mb->insertItem(tr("File"), fileMenu );

}

Simple1::Simple1( QWidget* parent, const char* name,  WFlags fl )
    : QWidget( parent, name, fl ) {

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setSpacing( 8 );
    layout->setMargin( 11 );


    QLabel *lbl = new QLabel( this, "a name for the label" );
    lbl->setText( tr("Click on the button or follow the white rabbit") );
    layout->addWidget( lbl );


    m_button = new QPushButton(this);


    m_button->setText( tr("Fire", "translatable quit string" ) );
    layout->addWidget( m_button );


    connect( m_button, SIGNAL(clicked() ),
             this, SLOT( slotFire() ) );
}

Simple1::~Simple1() {

}

void Simple1::slotFire() {
    /*
     * NOTE: Simple is now a child window of MainWindow
     * close will hide() Simple and not delete it. But as
     * the mainwindow is shown all children will be shown as well
     */
    close();
}


Simple2::Simple2( QWidget* parent, const char* name,  WFlags fl )
    : QWidget( parent, name, fl ) {

    /*
     * sets the caption of this toplevel widget
     * put all translatable string into tr()
     */
    setCaption(tr("My Simple Application") );

    /*
     * A simple vertical layout
     * either call layout->setAutoAdd( true )
     * or use layout->addWidget( wid ) to add widgets
     */
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setSpacing( 8 );
    layout->setMargin( 11 );

    /*
     * creates a label
     * The first parameter is this widget so the Label is a child
     * of us and will be deleted when we're deleted.
     */
    QLabel *lbl = new QLabel( this, "a name for the label" );
    /*
     * Resource will search hard for a Pixmap in $OPIEDIR/pics
     * to find 'logo/opielogo' You need to pass the subdir
     * but not the ending
     */
    lbl->setPixmap( Resource::loadPixmap("logo/opielogo") );
    layout->addWidget( lbl );


    /*  creates a button as child of this widget */
    m_button = new QPushButton(this);
    /*
     * another way to call tr. The first parameter is the string
     * to translate and the second a hint to the translator
     */
    m_button->setText( tr("Fire", "translatable fire string" ) );
    layout->addWidget( m_button );


    connect( m_button, SIGNAL(clicked() ),
             this, SLOT( slotQuit() ) );
}


Simple2::~Simple2() {

}

void Simple2::slotFire() {
    /*
     * We will fire up a sound
     * Note that Sound will use Resource as well
     * and we do not need to supply an ending
     * sounds are found in $OPIEDIR/sounds
     */
    Sound snd("hit_target01");
    snd.play();

}
