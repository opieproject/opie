#include <qaction.h> // action
#include <qmenubar.h> // menubar
#include <qtoolbar.h> // toolbar
#include <qlabel.h> // a label
#include <qpushbutton.h> // the header file for the QPushButton
#include <qlayout.h>

#include <qpe/qpeapplication.h> // the QPEApplication
#include <qpe/resource.h>

#include <opie/oapplicationfactory.h> // a template + macro to save the main method and allow quick launching

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
    setIcon( Resource::loadPixmap("new") );
    /*
     * out mainwindow should have a menubar
     * a toolbar, a mainwidget and use Resource
     * to get the IconSets
     */
    /*
     * we initialize the GUI in a different methid
     */
    initUI();

    Simple *simple = new Simple( this );
    setCentralWidget( simple );

    /*
     * If you use signal and slots do not include the parameter
     * names inside
     * so SIGNAL(fooBar(int) ) and NOT SIGNAL(fooBar(int foo) )
     */
    /*
     * We connect the activation of our QAction
     * to the slot connected to the firebutton
     * We could also connect the signal to the clicked
     * signal of the button
     */
    connect(m_fire, SIGNAL(activated() ),
            simple, SLOT(slotFire() ) );
}

MainWindow::~MainWindow() {
    // again nothing to delete because Qt takes care
}

/*
 * set Document is a special function used by Document
 * centric applications.
 * In example if Opie receives a Todo via IrDa it uses
 * setDocument via QCOP the IPC system to load the card
 * Or If you decide to open a file in filemanager with textedit
 * setDocument is called via IPC in textedit.
 * Also any call to QPE/Application/xyz and xyz is currently not running
 * leads to the start of xyz and delivering of the QCOP call
 * But more on QCOP in the next application
 */
void MainWindow::setDocument( const QString& /*str*/ ) {
    // in our case empty but you should see if it is a direct
    // file request or if it is a DocLnk.
    // A DocLnk is Link to an Document so you would end up
    // opening the document behind the file you got
}

/*
 * Two new concepts with this Method
 * 1. QAction. An action can be grouped and emits
 *      an activated signal. Action a universal useable
 *      you can just plug/addTo in most Qt widgets. For example
 *      the same action can be plugged into a ToolBar, MenuBar,
 *      QPopupMenu and other widgets but you do not need to worry
 *      about it
 *
 * 2. an IconSet contains pixmaps and provides them scaled down, scaled up
 *    enabled and disabled. SO if you use QIConSet and the toolbar
 *    size changes to use big pixmaps you will not use upscaled icons
 *    but the right ones thanks to QIconSet and Resource
 */

void MainWindow::initUI() {
/*
 * We want to provde a File Menu with Quit as option
 * and a Fire Toolbutton ( QAction )
 * So we need two actions
 * A toolbar and a popupMenu
 */
    setToolBarsMovable( false );
    /*
     *We don't want the static toolbar but share it with the
     * toolbar on small screens
     */
    QToolBar *menuBarHolder = new QToolBar( this );
    /* we allow the menubarholder to become bigger than
     * the screen width and to offer a > for the additional items
     */
    menuBarHolder->setHorizontalStretchable( true );
    QMenuBar *mb = new QMenuBar( menuBarHolder );
    QToolBar *tb = new QToolBar( this );

    QPopupMenu *fileMenu = new QPopupMenu( this );

    /*
     * we create our first action with the Text Quit
     * a IconSet, no menu name, no acceleration ( keyboard shortcut ),
     * with parent this, and name "quit_action"
     */
    /*
     * Note if you want a picture out of  the inline directory
     * you musn't prefix inline/ inline means these pics are built in
     * into libqpe so the name without ending and directory is enough
     */
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

Simple::Simple( QWidget* parent, const char* name,  WFlags fl )
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
    lbl->setText( tr("Click on the button or follow the white rabbit") );
    layout->addWidget( lbl );


    /*  creates a button as child of this widget */
    m_button = new QPushButton(this);
    /*
     * another way to call tr. The first parameter is the string
     * to translate and the second a hint to the translator
     */
    m_button->setText( tr("Fire", "translatable quit string" ) );
    layout->addWidget( m_button );

    /*
     * Now we bring the action into it. The power of qt is the dynamic
     * signal and slots model
     * Usage is simple connect m_buttons clicked signal to  our
     * slotQuit slot.
     * We could also have connected a SIGNAL to a SIGNAL or the clicked
     * signal directly to qApp and SLOT(quit() )
     */
    connect( m_button, SIGNAL(clicked() ),
             this, SLOT( slotFire() ) );
}

/*
 * Our destructor is empty because all child
 * widgets and layouts will be deleted by Qt.
 * Same applies to QObjects
 */
Simple::~Simple() {

}

void Simple::slotFire() {
    /*
     * NOTE: Simple is now a child window of MainWindow
     * close will hide() Simple and not delete it. But as
     * the mainwindow is shown all children will be shown as well
     */
    close();
}
