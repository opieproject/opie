/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "startmenu.h"
#include "inputmethods.h"
#include "runningappbar.h"
#include "systray.h"
#include "wait.h"
#include "appicons.h"

#include "taskbar.h"
#include "server.h"

#include <qtopia/config.h>
#include <qtopia/qpeapplication.h>
#ifdef QWS
#include <qtopia/qcopenvelope_qws.h>
#endif
#include <qtopia/global.h>
//#include <qtopia/custom.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#endif
#include <qwidgetstack.h>

#if defined( Q_WS_QWS )
#include <qwsdisplay_qws.h>
#include <qgfx_qws.h>
#endif


static bool initNumLock()
{
#ifdef QPE_INITIAL_NUMLOCK_STATE
    QPE_INITIAL_NUMLOCK_STATE
#endif
    return FALSE;
}

//---------------------------------------------------------------------------

class SafeMode : public QWidget
{
    Q_OBJECT
public:
    SafeMode( QWidget *parent ) : QWidget( parent ), menu(0)
    {
	message = tr("Safe Mode");
	QFont f( font() );
	f.setWeight( QFont::Bold );
	setFont( f );
    }

    void mousePressEvent( QMouseEvent *);
    QSize sizeHint() const;
    void paintEvent( QPaintEvent* );

private slots:
    void action(int i);

private:
    QString message;
    QPopupMenu *menu;
};

void SafeMode::mousePressEvent( QMouseEvent *)
{
    if ( !menu ) {
	menu = new QPopupMenu(this);
	menu->insertItem( tr("Plugin Manager..."), 0 );
	menu->insertItem( tr("Restart Qtopia"), 1 );
	menu->insertItem( tr("Help..."), 2 );
	connect(menu, SIGNAL(activated(int)), this, SLOT(action(int)));
    }
    QPoint curPos = mapToGlobal( QPoint(0,0) );
    QSize sh = menu->sizeHint();
    menu->popup( curPos-QPoint((sh.width()-width())/2,sh.height()) );
}

void SafeMode::action(int i)
{
    switch (i) {
	case 0:
	    Global::execute( "pluginmanager" );
	    break;
	case 1:
	    Global::restart();
	    break;
	case 2:
	    Global::execute( "helpbrowser", "safemode.html" );
	    break;
    }
}

QSize SafeMode::sizeHint() const
{
    QFontMetrics fm = fontMetrics();

    return QSize( fm.width(message), fm.height() );
}

void SafeMode::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    p.drawText( rect(), AlignCenter, message );
}

//---------------------------------------------------------------------------

class LockKeyState : public QWidget
{
public:
    LockKeyState( QWidget *parent ) :
	QWidget(parent),
	nl(initNumLock()), cl(FALSE)
    {
	nl_pm = Resource::loadPixmap("numlock");
	cl_pm = Resource::loadPixmap("capslock");
    }
    QSize sizeHint() const
    {
	return QSize(nl_pm.width()+2,nl_pm.width()+nl_pm.height()+1);
    }
    void toggleNumLockState()
    {
	nl = !nl; repaint();
    }
    void toggleCapsLockState()
    {
	cl = !cl; repaint();
    }
    void paintEvent( QPaintEvent * )
    {
	int y = (height()-sizeHint().height())/2;
	QPainter p(this);
	if ( nl )
	    p.drawPixmap(1,y,nl_pm);
	if ( cl )
	    p.drawPixmap(1,y+nl_pm.height()+1,cl_pm);
    }
private:
    QPixmap nl_pm, cl_pm;
    bool nl, cl;
};

//---------------------------------------------------------------------------

TaskBar::~TaskBar()
{
}


TaskBar::TaskBar() : QHBox(0, 0, WStyle_Customize | WStyle_Tool | WStyle_StaysOnTop | WGroupLeader)
{
    Config cfg( "Launcher" );
    cfg.setGroup( "InputMethods" );
    resizeRunningApp = cfg.readBoolEntry( "Resize", true );

    sm = new StartMenu( this );
    connect( sm, SIGNAL(tabSelected(const QString&)), this,
	    SIGNAL(tabSelected(const QString&)) );

    inputMethods = new InputMethods( this );
    connect( inputMethods, SIGNAL(inputToggled(bool)),
	     this, SLOT(calcMaxWindowRect()) );

    stack = new QWidgetStack( this );
    stack->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    label = new QLabel(stack);

    runningAppBar = new RunningAppBar(stack);
    stack->raiseWidget(runningAppBar);

    waitIcon = new Wait( this );
    (void) new AppIcons( this );

    sysTray = new SysTray( this );

    /* ### FIXME plugin loader and safe mode */
#if 0
    if (PluginLoader::inSafeMode())
	(void)new SafeMode( this );
#endif

    // ## make customizable in some way?
#ifdef QT_QWS_CUSTOM
    lockState = new LockKeyState( this );
#else
    lockState = 0;
#endif

#if defined(Q_WS_QWS)
#if !defined(QT_NO_COP)
    QCopChannel *channel = new QCopChannel( "QPE/TaskBar", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	this, SLOT(receive(const QCString&, const QByteArray&)) );
#endif
#endif
    waitTimer = new QTimer( this );
    connect( waitTimer, SIGNAL( timeout() ), this, SLOT( stopWait() ) );
    clearer = new QTimer( this );
    QObject::connect(clearer, SIGNAL(timeout()), SLOT(clearStatusBar()));

    connect( qApp, SIGNAL(symbol()), this, SLOT(toggleSymbolInput()) );
    connect( qApp, SIGNAL(numLockStateToggle()), this, SLOT(toggleNumLockState()) );
    connect( qApp, SIGNAL(capsLockStateToggle()), this, SLOT(toggleCapsLockState()) );
}

void TaskBar::setStatusMessage( const QString &text )
{
    if ( !text.isEmpty() ) {
	label->setText( text );
	stack->raiseWidget( label );
	if ( sysTray && ( label->fontMetrics().width( text ) > label->width() ) )
	    sysTray->hide();
	clearer->start( 3000, TRUE );
    } else {
	clearStatusBar();
    }
}

void TaskBar::clearStatusBar()
{
    label->clear();
    stack->raiseWidget(runningAppBar);
    if ( sysTray )
	sysTray->show();
    //     stack->raiseWidget( mru );
}

void TaskBar::startWait()
{
    waitIcon->setWaiting( true );
    // a catchall stop after 10 seconds...
    waitTimer->start( 10 * 1000, true );
}

void TaskBar::stopWait(const QString&)
{
    waitTimer->stop();
    waitIcon->setWaiting( false );
}

void TaskBar::stopWait()
{
    waitTimer->stop();
    waitIcon->setWaiting( false );
}

/*
 * This resizeEvent will be captured by
 * the ServerInterface and it'll layout
 * and calc rect. Now if we go from bigger
 * to smaller screen the SysTray is out of
 * bounds and repaint() won't trigger an Event
 */
void TaskBar::resizeEvent( QResizeEvent *e )
{
    if ( sysTray )
        sysTray->hide();

    QHBox::resizeEvent( e );

    if ( sysTray )
        sysTray->show();

    qWarning("TaskBar::resize event");
}

void TaskBar::styleChange( QStyle &s )
{
    QHBox::styleChange( s );
    calcMaxWindowRect();
}

void TaskBar::calcMaxWindowRect()
{
    if ( resizeRunningApp )
    {
    #if defined(Q_WS_QWS)
        QRect wr;
        int displayWidth  = qApp->desktop()->width();
        QRect ir = inputMethods->inputRect();
        if ( ir.isValid() ) {
            wr.setCoords( 0, 0, displayWidth-1, ir.top()-1 );
        } else {
            wr.setCoords( 0, 0, displayWidth-1, y()-1 );
        }
        #if QT_VERSION < 0x030000
        QWSServer::setMaxWindowRect( qt_screen->mapToDevice(wr,QSize(qt_screen->width(),qt_screen->height())) );
        #else
        QWSServer::setMaxWindowRect( wr );
        #endif
    #endif
    }
}

void TaskBar::receive( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "message(QString)" ) {
        QString text;
        stream >> text;
        setStatusMessage( text );
    } else if ( msg == "hideInputMethod()" ) {
	inputMethods->hideInputMethod();
    } else if ( msg == "showInputMethod()" ) {
	inputMethods->showInputMethod();
    } else if ( msg == "showInputMethod(QString)" ) {
        QString name;
        stream >> name;
	inputMethods->showInputMethod(name);
    } else if ( msg == "reloadInputMethods()" ) {
	inputMethods->loadInputMethods();
    } else if ( msg == "reloadApplets()" ) {
	sysTray->clearApplets();
	sm->createMenu();
	sysTray->addApplets();
    }else if ( msg == "toggleMenu()" ) {
        if ( sm-> launchMenu-> isVisible() )
            sm-> launch();
        else
            QCopEnvelope e( "QPE/System", "toggleApplicationMenu()" );
    }else if ( msg == "toggleStartMenu()" )
        sm->launch();
}

void TaskBar::setApplicationState( const QString &name, ServerInterface::ApplicationState state )
{
    if ( state == ServerInterface::Launching )
	runningAppBar->applicationLaunched( name );
    else if ( state == ServerInterface::Terminated )
	runningAppBar->applicationTerminated( name );
}

void TaskBar::toggleNumLockState()
{
    if ( lockState ) lockState->toggleNumLockState();
}

void TaskBar::toggleCapsLockState()
{
    if ( lockState ) lockState->toggleCapsLockState();
}

void TaskBar::toggleSymbolInput()
{
    QString unicodeInput = qApp->translate( "InputMethods", "Unicode" );
    if ( inputMethods->currentShown() == unicodeInput ) {
	inputMethods->hideInputMethod();
    } else {
	inputMethods->showInputMethod( unicodeInput );
    }
}

#include "taskbar.moc"
