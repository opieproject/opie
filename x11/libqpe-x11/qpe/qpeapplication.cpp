#define QTOPIA_INTERNAL_LANGLIST

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>


#include <qdir.h>
#include <qdialog.h>
#include <qdragobject.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlist.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qpalette.h>
#include <qptrdict.h>
#include <qregexp.h>
#include <qtimer.h>

#include <qpe/alarmserver.h>
#include <qpe/applnk.h>
#include <qpe/qpemenubar.h>
#include <qpe/textcodecinterface.h>
#include <qpe/imagecodecinterface.h>
#include <qpe/qlibrary.h>
#include <qpe/qpestyle.h>
#include <qpe/styleinterface.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/network.h>

#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/qcopenvelope_qws.h>


#include <X11/Xlib.h>
#include <X11/Xutil.h>

const int XKeyPress = KeyPress;
const int XKeyRelease = KeyRelease;
#undef KeyPress
#undef KeyRelease

namespace {
    struct QCopRec{
        QCopRec( const QCString& ch, const QCString& msg, const QByteArray& ar )
            : channel(ch), message(msg), data(ar) {

        }
        QCString channel;
        QCString message;
        QByteArray data;
    };
};


class QPEApplication::Private {
public:
    Private();
    ~Private();
    void enqueueQCop( const QCString& ch, const QCString& msg,
                      const QByteArray& );
    void sendQCopQ();
    static void show_mx(QWidget* mw, bool nomaximize );
    void show( QWidget* mw, bool nomax );
    void loadTextCodecs();
    void loadImageCodecs();

    int kbgrabber;
    int presstimer;

    bool rightpressed  : 1;
    bool kbregrab      : 1;
    bool notbusysent   : 1;
    bool preloaded     : 1;
    bool forceshow     : 1;
    bool nomaximize    : 1;
    bool keep_running  : 1;

    QWidget* presswidget;
    QPoint presspos;
    QWidget* qpe_main_widget;
    QString appName;
    QString styleName;
    QString decorationName;
    Atom wm_delete_window;
    Atom wm_take_focus;
    Atom wm_context_help;
    Atom wm_context_accept;
    Atom wm_protocols;

private:
    QList<QCopRec> qcopq;
};
QPEApplication::Private::~Private() {
}
QPEApplication::Private::Private()
    : kbgrabber(0 ), presstimer(0 ), rightpressed( FALSE ), kbregrab( FALSE ), notbusysent( FALSE ),
      preloaded( FALSE ), forceshow( FALSE ), nomaximize( FALSE ), keep_running( TRUE ),
      presswidget( 0 ), qpe_main_widget(0 ) {

    qcopq.setAutoDelete( TRUE );
}
void QPEApplication::Private::enqueueQCop( const QCString& chan, const QCString& msg,
                                           const QByteArray& ar ) {
    qcopq.append( new QCopRec(chan, msg, ar ) );
}
void QPEApplication::Private::sendQCopQ() {
    QCopRec* r;
    for ( r = qcopq.first(); r; r = qcopq.next() ) {
        QCopChannel::sendLocally( r->channel, r->message, r->data );
    }
    qcopq.clear();
}
void QPEApplication::Private::show_mx(QWidget* mw, bool nomaximize ) {
    if (mw->layout() && mw->inherits("QDialog") ) {
        QPEApplication::showDialog( (QDialog*)mw, nomaximize );
    }else {
        if (!nomaximize )
            mw->showMaximized();
        else
            mw->show();
    }
}
void QPEApplication::Private::show( QWidget* mw, bool nomax ) {
    nomaximize = nomax;
    qpe_main_widget = mw;

    sendQCopQ();

    if ( preloaded ) {
        if (forceshow )
            show_mx(mw, nomax );
    }else if ( keep_running )
        show_mx( mw, nomax );
}
void QPEApplication::Private::loadTextCodecs() {
    QString path = QPEApplication::qpeDir() + "/plugins/textcodecs";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        TextCodecInterface *iface = 0;
        QLibrary *lib = new QLibrary( path + "/" + *it );
        if ( lib->queryInterface( IID_QtopiaTextCodec, (QUnknownInterface**)&iface ) == QS_OK && iface ) {
            QValueList<int> mibs = iface->mibEnums();
            for (QValueList<int>::ConstIterator i = mibs.begin(); i != mibs.end(); ++i) {
                (void)iface->createForMib(*i);
                // ### it exists now; need to remember if we can delete it
            }
        }
        else {
            lib->unload();
            delete lib;
        }
    }
}
void QPEApplication::Private::loadImageCodecs() {
    QString path = QPEApplication::qpeDir() + "/plugins/imagecodecs";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        ImageCodecInterface *iface = 0;
        QLibrary *lib = new QLibrary( path + "/" + *it );
        if ( lib->queryInterface( IID_QtopiaImageCodec, (QUnknownInterface**)&iface ) == QS_OK && iface ) {
            QStringList formats = iface->keys();
            for (QStringList::ConstIterator i = formats.begin(); i != formats.end(); ++i) {
                (void)iface->installIOHandler(*i);
                // ### it exists now; need to remember if we can delete it
            }
        }
        else {
            lib->unload();
            delete lib;
        }
    }
}

// The Help System hook
namespace {
    class ResourceMimeFactory : public QMimeSourceFactory
    {
    public:
        ResourceMimeFactory();
        ~ResourceMimeFactory();
        const QMimeSource* data( const QString& abs_name )const;
    };
    ResourceMimeFactory::ResourceMimeFactory()
    {
        setFilePath( Global::helpPath() );
        setExtensionType( "html", "text/html;charset=UTF-8" );
    }
    ResourceMimeFactory::~ResourceMimeFactory() {
    }

    const QMimeSource* ResourceMimeFactory::data( const QString& abs_name ) const
    {
        const QMimeSource * r = QMimeSourceFactory::data( abs_name );
        if ( !r ) {
            int sl = abs_name.length();
            do {
                sl = abs_name.findRev( '/', sl - 1 );
                QString name = sl >= 0 ? abs_name.mid( sl + 1 ) : abs_name;
                int dot = name.findRev( '.' );
                if ( dot >= 0 )
                    name = name.left( dot );
                QImage img = Resource::loadImage( name );
                if ( !img.isNull() )
                    r = new QImageDrag( img );
            }
            while ( !r && sl > 0 );
        }
        return r;
    };
};
// QPEApplication
QPEApplication::~QPEApplication() {
    qWarning("~QPEApplication");
    ungrabKeyboard();
    qWarning("UngrabKeyboard");

//    delete m_sys;
//    delete m_pid;

    delete d;
}
QPEApplication::QPEApplication(int &arg, char** argv, Type t)
    : QApplication( arg, argv, t ) {
    d = new Private;
    d->loadTextCodecs();
    d->loadImageCodecs();

    // Init X-Atom
    Atom *atoms[5];
    Atom atoms_re[5];
    char* names[5];
    int n = 0;
    atoms[n]   = &d->wm_delete_window;
    names[n++] = "WM_DELETE_WINDOW";

    atoms[n]   = &d->wm_take_focus;
    names[n++] = "WM_TAKE_FOCUS";

    atoms[n]   = &d->wm_context_help;
    names[n++] = "_NET_WM_CONTEXT_HELP";

    atoms[n]   = &d->wm_context_accept;
    names[n++] = "_NET_WM_CONTEXT_ACCEPT";

    atoms[n]   = &d->wm_protocols;
    names[n++] = "WM_PROTOCOLS";

    XInternAtoms( qt_xdisplay(), names, n, FALSE, atoms_re);
    // now copy the values over to the properties
    for (int i = 0; i < n; i++ )
        *atoms[i] = atoms_re[i];
    // done with X11 Stuff

    int dw = desktop()->width();
    if ( dw < 200 ) {
        setFont( QFont( "helvetica", 8 ) );
        AppLnk::setSmallIconSize( 10 );
        AppLnk::setBigIconSize( 28 );
    }else if ( dw > 600 ) {
        setFont( QFont( "helvetica", 12 ) );
        AppLnk::setSmallIconSize( 24 );
        AppLnk::setBigIconSize( 48 );
    }else if ( dw > 200 ) {
        setFont( QFont( "helvetica", 10 ) );
        AppLnk::setSmallIconSize( 16 );
        AppLnk::setBigIconSize( 32 );
    }
    QMimeSourceFactory::setDefaultFactory( new ResourceMimeFactory );

    connect( this, SIGNAL( lastWindowClosed() ), this, SLOT(hideOrQuit() ) );

    QString qcopfn( "/tmp/qcop-msg-" );
    qcopfn += QString( argv[0] ); // append command name to the QCOP name
    QFile file( qcopfn );
    if (file.open(IO_ReadOnly ) ) {
        flock( file.handle(), LOCK_EX );
    }

    /* Hmmm damn we need to make the parent 0l otherwise it get's deleted
     * past the QApplication
     */
    m_sys = new QCopChannel( "QPE/System", 0l);
    connect(m_sys, SIGNAL( received( const QCString&, const QByteArray& ) ),
            this, SLOT(systemMessage( const QCString&, const QByteArray& ) ) );

    // private channel QPE/Application/appname
    QCString channel = QCString( argv[0] );
    channel.replace( QRegExp( ".*/"), "" );
    d->appName = channel;
    channel = "QPE/Application/"+ channel;
    m_pid = new QCopChannel( channel, 0l );
    connect(m_pid, SIGNAL( received( const QCString&, const QByteArray& ) ),
            this, SLOT( pidMessage( const QCString&, const QByteArray& ) ) );

    // read the Pre QCOP Stuff  from the file
    if ( file.isOpen() ) {
        d->keep_running = FALSE;
        QDataStream ds( &file );
        QCString chanel, message;
        QByteArray data;
        while (!ds.atEnd() ) {
            ds >> chanel >> message >> data;
            d->enqueueQCop( chanel, message, data );
        }
        flock( file.handle(), LOCK_UN );
        file.close();
        file.remove();
    }

    // read in some stuff from the command line
    // we do not have setArgs so we need to take
    // care of that
    for ( int a = 0; a < arg; a++ ) {
        if ( qstrcmp( argv[a], "-preload" ) == 0 ) {
            d->preloaded = TRUE;
        }else if ( qstrcmp( argv[a ] ,  "-preload-show" ) == 0 ) {
            d->preloaded = TRUE;
            d->forceshow = TRUE;
        }
    }
    initTranslations();
    applyStyle();

    if ( type() == GuiServer )
        ;

    installEventFilter( this );
    QPEMenuToolFocusManager::initialize();
}
void QPEApplication::initTranslations() {
    // Translations add it
    QStringList langs = Global::languageList();
    for ( QStringList::ConstIterator it = langs.begin(); it != langs.end(); ++it ) {
        QString lang = *it;

        QTranslator * trans;
        QString tfn;

        trans = new QTranslator( this );
        tfn = qpeDir() + "/i18n/" + lang + "/libqpe.qm";
        if ( trans->load( tfn ) )
            installTranslator( trans );
        else
            delete trans;

        trans = new QTranslator( this );
        tfn = qpeDir() + "/i18n/" + lang + "/" + d->appName + ".qm";
        if ( trans->load( tfn ) )
            installTranslator( trans );
        else
            delete trans;
    }
}
QString QPEApplication::qpeDir() {
    const char * base = getenv( "OPIEDIR" );
    if ( base )
        return QString( base ) + "/";

    return QString( "../" );
}
QString QPEApplication::documentDir() {
    const char* base = getenv( "HOME");
    if ( base )
        return QString( base ) + "/Documents";

    return QString( "../Documents" );
}
void QPEApplication::applyStyle() {
    Config config( "qpe" );

    config.setGroup( "Appearance" );

    // Widget style
    QString style = config.readEntry( "Style", "Light" );
    internalSetStyle( style );

    // Colors
    QColor bgcolor( config.readEntry( "Background", "#E5E1D5" ) );
    QColor btncolor( config.readEntry( "Button", "#D6CDBB" ) );
    QPalette pal( btncolor, bgcolor );
    QString color = config.readEntry( "Highlight", "#800000" );
    pal.setColor( QColorGroup::Highlight, QColor( color ) );
    color = config.readEntry( "HighlightedText", "#FFFFFF" );
    pal.setColor( QColorGroup::HighlightedText, QColor( color ) );
    color = config.readEntry( "Text", "#000000" );
    pal.setColor( QColorGroup::Text, QColor( color ) );
    color = config.readEntry( "ButtonText", "#000000" );
    pal.setColor( QPalette::Active, QColorGroup::ButtonText, QColor( color ) );
    color = config.readEntry( "Base", "#FFFFFF" );
    pal.setColor( QColorGroup::Base, QColor( color ) );

    pal.setColor( QPalette::Disabled, QColorGroup::Text,
                  pal.color( QPalette::Active, QColorGroup::Background ).dark() );

    setPalette( pal, TRUE );



    // Font
    QString ff = config.readEntry( "FontFamily", font().family() );
    int fs = config.readNumEntry( "FontSize", font().pointSize() );
    setFont( QFont(ff, fs) );
}
int QPEApplication::defaultRotation() {
    return 0;
}
void QPEApplication::setDefaultRotation(int r ) {

}
void QPEApplication::grabKeyboard() {
    QPEApplication::Private * d = ( ( QPEApplication* ) qApp ) ->d;
    if ( qApp->type() == QApplication::GuiServer )
        d->kbgrabber = 0;
    else {
        QCopEnvelope e( "QPE/System", "grabKeyboard(QString)" );
        e << d->appName;

        d->kbgrabber = 2; // me
    }
}
void QPEApplication::ungrabKeyboard() {
    QPEApplication::Private * d = ( ( QPEApplication* ) qApp ) ->d;
    if ( d->kbgrabber == 2 ) {
        QCopEnvelope e( "QPE/System", "grabKeyboard(QString)" );
        e << QString::null;

        d->kbregrab = FALSE;
        d->kbgrabber = 0;
    }
}
void QPEApplication::showMainWidget( QWidget* wid, bool b) {
    d->show(wid, b );
}
void QPEApplication::showMainDocumentWidget( QWidget* mw, bool m) {
    if ( mw && argc() == 2 )
        Global::setDocument( mw, QString::fromUtf8(argv()[1] ) );

    d->show(mw, m );
}
void QPEApplication::showDialog( QDialog* d, bool nomax ) {
    QSize sh = d->sizeHint();
    int w = QMAX(sh.width(),d->width());
    int h = QMAX(sh.height(),d->height());
    if ( !nomax
         && ( w > qApp->desktop()->width()*3/4
              || h > qApp->desktop()->height()*3/4 ) )
    {
	d->showMaximized();
    } else {
	d->resize(w,h);
	d->show();
    }
}
int QPEApplication::execDialog( QDialog* d, bool nomax) {
    showDialog(d,nomax);
    return d->exec();
}
void QPEApplication::setKeepRunning() {
    if ( qApp && qApp->inherits( "QPEApplication" ) ) {
        QPEApplication * qpeApp = ( QPEApplication* ) qApp;
        qpeApp->d->keep_running = TRUE;
    }
}
bool QPEApplication::keepRunning()const {
    return d->keep_running;
}
bool QPEApplication::keyboardGrabbed()const {
    return d->kbgrabber;
}
int QPEApplication::exec() {
    /* now send the QCOP stuff gotten from the file */
    d->sendQCopQ();

    if ( d->keep_running ) {
        qWarning("going to exec");
        int a = QApplication::exec();
        qWarning("left");
        return a;
    }

    {
        QCopEnvelope e( "QPE/System", "closing(QString)" );
        e << d->appName;
    }
    qWarning("processing events!");
    processEvents();
    return 0;
}
void QPEApplication::internalSetStyle( const QString& ) {

}
void QPEApplication::systemMessage( const QCString&, const QByteArray& ) {

}
void QPEApplication::pidMessage( const QCString&, const QByteArray& ) {

}
void QPEApplication::timerEvent( QTimerEvent* e ) {
    if ( e->timerId() == d->presstimer && d->presswidget ) {
        // Right pressed
        postEvent( d->presswidget,
                   new QMouseEvent( QEvent::MouseButtonPress, d->presspos,
                                    RightButton, LeftButton ) );
        killTimer( d->presstimer );
        d->presstimer = 0;
    }
}

// InputMethods Hints
namespace {
    static QPtrDict<void>* inputMethodDict = 0;
    static void createInputMethodDict(){
	if ( !inputMethodDict )
            inputMethodDict = new QPtrDict<void>;
    }

    static QPtrDict<void>* stylusDict = 0;
    static void createDict() {
        if ( !stylusDict )
            stylusDict = new QPtrDict<void>;
    }
};

void QPEApplication::setInputMethodHint( QWidget* w, InputMethodHint mode ) {
    createInputMethodDict();
    if ( mode == Normal ) {
        inputMethodDict->remove
            ( w );
    }else {
        inputMethodDict->insert( w, ( void* ) mode );
    }
}
QPEApplication::InputMethodHint QPEApplication::inputMethodHint( QWidget* w) {
    if ( inputMethodDict && w )
        return ( InputMethodHint ) ( int ) inputMethodDict->find( w );
    return Normal;
}


void QPEApplication::removeSenderFromStylusDict() {
    stylusDict->remove( ( void* ) sender() );
    if ( d->presswidget == sender() )
        d->presswidget = 0;
}
void QPEApplication::setStylusOperation( QWidget* w, StylusMode mode) {
    createDict();
    if ( mode == LeftOnly ) {
        stylusDict->remove
            ( w );
        w->removeEventFilter( qApp );
    }else {
        stylusDict->insert( w, ( void* ) mode );
        connect( w, SIGNAL( destroyed() ), qApp, SLOT( removeSenderFromStylusDict() ) );
        w->installEventFilter( qApp );
    }
}
QPEApplication::StylusMode QPEApplication::stylusOperation( QWidget* w) {
    if ( stylusDict )
        return ( StylusMode ) ( int ) stylusDict->find( w );
    return LeftOnly;
}

// eventFilter......
bool QPEApplication::eventFilter( QObject* o, QEvent* e ) {
    /*
     * We want our WM to show Ok and a X button
     * on dialogs
     * our part is to set the _NET_WM_CONTEXT_ACCEPT
     * propery
     * and then wait for a client message -zecke
     * on show we will add the prop
     */
    if (o->inherits("QDialog") && e->type() == QEvent::Show ) {
        QDialog* dialog = (QDialog*)o;
        Atom wm_prot[45];
        int n = 0;
        wm_prot[n++] = d->wm_delete_window;
        wm_prot[n++] = d->wm_take_focus;
        wm_prot[n++] = d->wm_context_accept;
        if ( dialog->testWFlags( WStyle_ContextHelp ) )
            wm_prot[n++] = d->wm_context_help;
        XSetWMProtocols( qt_xdisplay(), dialog->winId(), wm_prot, n );
        return TRUE; // should be save
    }
    if ( stylusDict && e->type() >= QEvent::MouseButtonPress && e->type() <= QEvent::MouseMove ) {
        QMouseEvent * me = ( QMouseEvent* ) e;
        StylusMode mode = (StylusMode)(int)stylusDict->find(o);
        switch (mode) {
        case RightOnHold:
            switch ( me->type() ) {
            case QEvent::MouseButtonPress:
                if ( me->button() == LeftButton ) {
                    d->presstimer = startTimer(500); // #### pref.
                    d->presswidget = (QWidget*)o;
                    d->presspos = me->pos();
                    d->rightpressed = FALSE;
                }
                break;
            case QEvent::MouseMove:
                if (d->presstimer && (me->pos() - d->presspos).manhattanLength() > 8) {
                    killTimer(d->presstimer);
                    d->presstimer = 0;
                }
                break;
            case QEvent::MouseButtonRelease:
                if ( me->button() == LeftButton ) {
                    if ( d->presstimer ) {
                        killTimer(d->presstimer);
                        d->presstimer = 0;
                    }
                    if ( d->rightpressed && d->presswidget ) {
                        // Right released
                        postEvent( d->presswidget,
                                   new QMouseEvent( QEvent::MouseButtonRelease, me->pos(),
                                                    RightButton, LeftButton + RightButton ) );
                        // Left released, off-widget
                        postEvent( d->presswidget,
                                   new QMouseEvent( QEvent::MouseMove, QPoint( -1, -1),
                                                    LeftButton, LeftButton ) );
                        postEvent( d->presswidget,
                                   new QMouseEvent( QEvent::MouseButtonRelease, QPoint( -1, -1),
                                                    LeftButton, LeftButton ) );
                        d->rightpressed = FALSE;
                        return TRUE; // don't send the real Left release
                    }
                }
                break;
            default:
                break;
            }
            break;
        default:
            ;
        }
    }
    else if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease ) {
        QKeyEvent *ke = (QKeyEvent *)e;
        if ( ke->key() == Key_Enter ) {
            if ( o->isA( "QRadioButton" ) || o->isA( "QCheckBox" ) ) {
                postEvent( o, new QKeyEvent( e->type(), Key_Space, ' ',
                                             ke->state(), " ", ke->isAutoRepeat(), ke->count() ) );
                return TRUE;
            }
        }
    }
    return FALSE;
}

// Quit stuff
void QPEApplication::restart() {

}
void QPEApplication::shutdown() {

}
void QPEApplication::tryQuit() {
    qWarning("TryQuit!!");
    if ( activeModalWidget() || strcmp( argv() [ 0 ], "embeddedkonsole" ) == 0 )
        return ; // Inside modal loop or konsole. Too hard to save state.
    {
        QCopEnvelope e( "QPE/System", "closing(QString)" );
        e << d->appName;
    }
    processEvents();

    quit();
}
void QPEApplication::hideOrQuit() {
    qWarning("hide or close");
    processEvents();
    qWarning("past processing");

    // If we are a preloaded application we don't actually quit, so emit
    // a System message indicating we're quasi-closing.
    if ( d->preloaded && d->qpe_main_widget )

    {
        qWarning("hiding");
        QCopEnvelope e("QPE/System", "fastAppHiding(QString)" );
        e << d->appName;
        d->qpe_main_widget->hide();
    }
    else
        quit();
}

/*!
  \internal
*/
void QPEApplication::prepareForTermination( bool willrestart )
{
	if ( willrestart ) {
		// Draw a big wait icon, the image can be altered in later revisions
		//  QWidget *d = QApplication::desktop();
		QImage img = Resource::loadImage( "launcher/new_wait" );
		QPixmap pix;
		pix.convertFromImage( img.smoothScale( 1 * img.width(), 1 * img.height() ) );
		QLabel *lblWait = new QLabel( 0, "wait hack!", QWidget::WStyle_Customize |
		                              QWidget::WStyle_NoBorder | QWidget::WStyle_Tool );
		lblWait->setPixmap( pix );
		lblWait->setAlignment( QWidget::AlignCenter );
		lblWait->show();
		lblWait->showMaximized();
	}
#ifndef SINGLE_APP
	{ QCopEnvelope envelope( "QPE/System", "forceQuit()" );
	}
	processEvents(); // ensure the message goes out.
	sleep( 1 ); // You have 1 second to comply.
#endif
}
int QPEApplication::x11ClientMessage(QWidget* w, XEvent* event, bool b ) {
    qWarning("X11 ClientMessage %d %d", event->type,  ClientMessage);
    if ( event->type == ClientMessage ) {
        if ( (event->xclient.message_type == d->wm_protocols) &&
             (event->xclient.data.l[0] == d->wm_context_accept ) ) {
            qWarning("accepted!!!");
            /*
             * I'm not sure if we should use activeWidget
             * or activeModalWidget
             * a QDialog could be not modal too
             */
            if ( w->inherits("QDialog" ) ) {
                qWarning("inherits QDialog!!!");
                QDialog* dia = (QDialog*)w;
                /*
                 * call it directly or via QTimer::singleShot?
                 */
                QTimer::singleShot(0, dia, SLOT(reject() ) );
                 return 0;
            }

        }
    }
    return QApplication::x11ClientMessage(w, event, b );
}

#define KeyPress XKeyPress
#define KeyRelease XKeyRelease

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX)

// The libraries with the skiff package (and possibly others) have
// completely useless implementations of builtin new and delete that
// use about 50% of your CPU. Here we revert to the simple libc
// functions.

void* operator new[]( size_t size )
{
	return malloc( size );
}

void* operator new( size_t size )
{
	return malloc( size );
}

void operator delete[]( void* p )
{
	free( p );
}

void operator delete[]( void* p, size_t /*size*/ )
{
	free( p );
}

void operator delete( void* p )
{
	free( p );
}

void operator delete( void* p, size_t /*size*/ )
{
	free( p );
}

#endif
