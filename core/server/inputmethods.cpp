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

#define QTOPIA_INTERNAL_LANGLIST
#include "inputmethods.h"

#include <qtopia/config.h>
#include <qtopia/qpeapplication.h>
#include <qtopia/inputmethodinterface.h>
#include <qtopia/global.h>

#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qdir.h>
#include <stdlib.h>
#include <qtranslator.h>
#include <qtl.h>

#ifdef Q_WS_QWS
#include <qwindowsystem_qws.h>
#include <qwsevent_qws.h>
#include <qcopchannel_qws.h>
#endif

/* ### SingleFloppy if someone is interested? */
#if 0
#ifdef QT_NO_COMPONENT
#include "../plugins/inputmethods/handwriting/handwritingimpl.h"
#include "../plugins/inputmethods/keyboard/keyboardimpl.h"
#include "../3rdparty/plugins/inputmethods/pickboard/pickboardimpl.h"
#endif
#endif

/* XPM */
static const char * tri_xpm[]={
"9 9 2 1",
"a c #000000",
". c None",
".........",
".........",
".........",
"....a....",
"...aaa...",
"..aaaaa..",
".aaaaaaa.",
".........",
"........."};

static const int inputWidgetStyle = QWidget::WStyle_Customize |
				    QWidget::WStyle_Tool |
				    QWidget::WStyle_StaysOnTop |
				    QWidget::WGroupLeader;


int InputMethod::operator <(const InputMethod& o) const
{
    return name() < o.name();
}
int InputMethod::operator >(const InputMethod& o) const
{
    return name() > o.name();
}
int InputMethod::operator <=(const InputMethod& o) const
{
    return name() <= o.name();
}


/*
  Slightly hacky: We use WStyle_Tool as a flag to say "this widget
  belongs to the IM system, so clicking it should not cause a reset".
 */
class IMToolButton : public QToolButton
{
public:
    IMToolButton::IMToolButton( QWidget *parent ) : QToolButton( parent )
    { setWFlags( WStyle_Tool ); }
};


InputMethods::InputMethods( QWidget *parent ) :
    QWidget( parent, "InputMethods", WStyle_Tool | WStyle_Customize ),
    mkeyboard(0), imethod(0)
{
    setBackgroundMode( PaletteBackground );
    QHBoxLayout *hbox = new QHBoxLayout( this );

    kbdButton = new IMToolButton( this);
    kbdButton->setFocusPolicy(NoFocus);
    kbdButton->setToggleButton( TRUE );
    if (parent->sizeHint().height() > 0)
	kbdButton->setFixedHeight( parent->sizeHint().height() );
    kbdButton->setFixedWidth( 32 );
    kbdButton->setAutoRaise( TRUE );
    kbdButton->setUsesBigPixmap( TRUE );
    hbox->addWidget( kbdButton );
    connect( kbdButton, SIGNAL(toggled(bool)), this, SLOT(showKbd(bool)) );

    kbdChoice = new IMToolButton( this );
    kbdChoice->setFocusPolicy(NoFocus);
    kbdChoice->setPixmap( QPixmap( (const char **)tri_xpm ) );
    if (parent->sizeHint().height() > 0)
	kbdChoice->setFixedHeight( parent->sizeHint().height() );
    kbdChoice->setFixedWidth( 13 );
    kbdChoice->setAutoRaise( TRUE );
    hbox->addWidget( kbdChoice );
    connect( kbdChoice, SIGNAL(clicked()), this, SLOT(chooseKbd()) );

    connect( (QPEApplication*)qApp, SIGNAL(clientMoused()),
	    this, SLOT(resetStates()) );


    imButton = new QWidgetStack( this ); // later a widget stack
    imButton->setFocusPolicy(NoFocus);
    if (parent->sizeHint().height() > 0)
	imButton->setFixedHeight( parent->sizeHint().height() );
    hbox->addWidget(imButton);

    imChoice = new QToolButton( this );
    imChoice->setFocusPolicy(NoFocus);
    imChoice->setPixmap( QPixmap( (const char **)tri_xpm ) );
    if (parent->sizeHint().height() > 0)
	imChoice->setFixedHeight( parent->sizeHint().height() );
    imChoice->setFixedWidth( 13 );
    imChoice->setAutoRaise( TRUE );
    hbox->addWidget( imChoice );
    connect( imChoice, SIGNAL(clicked()), this, SLOT(chooseIm()) );

    loadInputMethods();

    QCopChannel *channel = new QCopChannel( "QPE/IME", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	     this, SLOT(qcopReceive(const QCString&, const QByteArray&)) );
}

InputMethods::~InputMethods()
{
    Config cfg("qpe");
    cfg.setGroup("InputMethod");
    if (imethod)
        cfg.writeEntry("im", imethod->name() );
    if (mkeyboard)
        cfg.writeEntry("current", mkeyboard->name() );

    unloadInputMethods();
}

void InputMethods::hideInputMethod()
{
    kbdButton->setOn( FALSE );
}

void InputMethods::showInputMethod()
{
    kbdButton->setOn( TRUE );
}

void InputMethods::showInputMethod(const QString& name)
{
    int i = 0;
    QValueList<InputMethod>::Iterator it;
    InputMethod *im = 0;
    for ( it = inputMethodList.begin(); it != inputMethodList.end(); ++it, i++ ) {
	QString lname = (*it).libName.mid((*it).libName.findRev('/') + 1);
	if ( (*it).name() == name || lname == name ) {
	    im = &(*it);
	    break;
	}
    }
    if ( im )
	chooseKeyboard(im);
}

void InputMethods::resetStates()
{
    if ( mkeyboard && !mkeyboard->newIM )
	mkeyboard->interface->resetState();
}

QRect InputMethods::inputRect() const
{
    if ( !mkeyboard || !mkeyboard->widget || !mkeyboard->widget->isVisible() )
	return QRect();
    else
	return mkeyboard->widget->geometry();
}

void InputMethods::unloadInputMethods()
{
    unloadMethod( inputMethodList );
    unloadMethod( inputModifierList );
    inputMethodList.clear();
    inputModifierList.clear();

}

void InputMethods::unloadMethod( QValueList<InputMethod>& list ) {
    QValueList<InputMethod>::Iterator it;

    for (it = list.begin(); it != list.end(); ++it )
        (*it).releaseInterface();

}


QStringList InputMethods::plugins()const {
   QString path = QPEApplication::qpeDir() + "/plugins/inputmethods";
   QDir dir( path, "lib*.so" );
   return dir.entryList();
}

void InputMethods::installTranslator( const QString& type ) {
    QStringList langs = Global::languageList();
    QStringList::ConstIterator lit;
    for ( lit= langs.begin(); lit!=langs.end(); ++lit) {
        QString lang = *lit;
        QTranslator * trans = new QTranslator(qApp);

        QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";

        if ( trans->load( tfn ))
            qApp->installTranslator( trans );
        else
            delete trans;
    }
}

void InputMethods::setPreferedHandlers() {
    Config cfg("qpe");
    cfg.setGroup("InputMethod");
    QString current = cfg.readEntry("current");
    QString im = cfg.readEntry("im");

    QValueList<InputMethod>::Iterator it;
    if (!inputModifierList.isEmpty() && !im.isEmpty() ) {
        for (it = inputModifierList.begin(); it != inputModifierList.end(); ++it )
            if ( (*it).name() == im ) {
                imethod = &(*it); break;
            }

    }
    if (!inputMethodList.isEmpty() && !current.isEmpty() ) {
        for (it = inputMethodList.begin(); it != inputMethodList.end(); ++it )
            if ( (*it).name() == current ) {
                qWarning("preferred keyboard is %s", current.latin1() );
                mkeyboard = &(*it);
                kbdButton->setPixmap( *mkeyboard->icon() );
                break;
            }
    }

}

void InputMethods::loadInputMethods()
{
#ifndef QT_NO_COMPONENT
    hideInputMethod();
    mkeyboard = 0;

    unloadInputMethods();

    QString path = QPEApplication::qpeDir() + "/plugins/inputmethods";
    QStringList list = plugins();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
	InputMethodInterface *iface = 0;
	ExtInputMethodInterface *eface = 0;
        QLibrary *lib = new QLibrary( path + "/" + *it );

	if ( lib->queryInterface( IID_InputMethod, (QUnknownInterface**)&iface ) == QS_OK ) {
	    InputMethod input;
	    input.newIM = FALSE;
            input.library = lib;
	    input.libName = *it;
	    input.interface = iface;
	    input.widget = input.interface->inputMethod( 0, inputWidgetStyle );
	    input.interface->onKeyPress( this, SLOT(sendKey(ushort,ushort,ushort,bool,bool)) );
	    inputMethodList.append( input );
	} else if ( lib->queryInterface( IID_ExtInputMethod, (QUnknownInterface**)&eface ) == QS_OK ) {
	    InputMethod input;
	    input.newIM = TRUE;
            input.library = lib;
	    input.libName = *it;
	    input.extInterface = eface;
	    input.widget = input.extInterface->keyboardWidget( 0, inputWidgetStyle );
	    // may be either a simple, or advanced.
	    if (input.widget) {
		//qDebug("its a keyboard");
		inputMethodList.append( input );
	    } else {
		//qDebug("its a real im");
		input.widget = input.extInterface->statusWidget( 0, 0 );
		if (input.widget) {
		    //qDebug("blah");
		    inputModifierList.append( input );
		    imButton->addWidget(input.widget, inputModifierList.count());
		}
	    }
	}else{
            delete lib;
            lib = 0l;
        }
        installTranslator(  (*it).left( (*it).find(".") ) );
    }
    qHeapSort( inputMethodList );
#endif /* killed BUILT in cause they would not compile */

    QWSServer::setCurrentInputMethod( 0 );

    /* set the prefered IM + handler */
    setPreferedHandlers();
    if ( !inputModifierList.isEmpty() ) {
        if (!imethod)
            imethod = &inputModifierList[0];
	imButton->raiseWidget(imethod->widget);
	QWSServer::setCurrentInputMethod( imethod->extInterface->inputMethod() );
    } else {
	imethod = 0;
    }

    // we need to update keyboards afterwards, as some of them may not be compatible with
    // the current input method
    updateKeyboards(imethod);

    if ( !inputModifierList.isEmpty() )
	imButton->show();
    else
	imButton->hide();

    if ( inputModifierList.count() > 1 )
	imChoice->show();
    else
	imChoice->hide();
}

void InputMethods::chooseKbd()
{
    QPopupMenu pop( this );
    pop.setFocusPolicy( NoFocus ); //don't reset IM

    QString imname;
    if (imethod)
	imname = imethod->libName.mid(imethod->libName.findRev('/') + 1);

    int i = 0;
    int firstDepKbd = 0;

    QValueList<InputMethod>::Iterator it;
    for ( it = inputMethodList.begin(); it != inputMethodList.end(); ++it, i++ ) {
	// add empty new items, all old items.
	if (!(*it).newIM || (*it).extInterface->compatible().count() == 0 ) {
	    pop.insertItem( (*it).name(), i, firstDepKbd);
	    if ( mkeyboard == &(*it) )
		pop.setItemChecked( i, TRUE );

	    firstDepKbd++;
	} else if ( (*it).extInterface->compatible().contains(imname)) {
	    // check if we need to insert a sep.
	    if (firstDepKbd == i)
		pop.insertSeparator();
	    pop.insertItem( (*it).name(), i, -1);
	    if ( mkeyboard == &(*it) )
		pop.setItemChecked( i, TRUE );
	}
    }

    QPoint pt = mapToGlobal(kbdChoice->geometry().topRight());
    QSize s = pop.sizeHint();
    pt.ry() -= s.height();
    pt.rx() -= s.width();
    i = pop.exec( pt );
    if ( i == -1 )
	return;
    InputMethod *im = &inputMethodList[i];
    chooseKeyboard(im);
}

void InputMethods::chooseIm()
{
    QPopupMenu pop( this );

    int i = 0;
    QValueList<InputMethod>::Iterator it;
    for ( it = inputModifierList.begin(); it != inputModifierList.end(); ++it, i++ ) {
	pop.insertItem( (*it).name(), i );
	if ( imethod == &(*it) )
	    pop.setItemChecked( i, TRUE );
    }

    QPoint pt = mapToGlobal(imChoice->geometry().topRight());
    QSize s = pop.sizeHint();
    pt.ry() -= s.height();
    pt.rx() -= s.width();
    i = pop.exec( pt );
    if ( i == -1 )
	return;
    InputMethod *im = &inputModifierList[i];

    chooseMethod(im);
}

void InputMethods::chooseKeyboard(InputMethod* im)
{
    if ( im != mkeyboard ) {
	if ( mkeyboard && mkeyboard->widget->isVisible() )
	    mkeyboard->widget->hide();
	mkeyboard = im;
	kbdButton->setPixmap( *mkeyboard->icon() );
    }
    if ( !kbdButton->isOn() )
	kbdButton->setOn( TRUE );
    else
	showKbd( TRUE );
}

static bool keyboardCompatible(InputMethod *keyb, const QString &imname )
{
    if ( !keyb || !keyb->newIM || !keyb->extInterface->compatible().count() )
	return TRUE;

    if ( keyb->extInterface->compatible().contains(imname) )
	return TRUE;

    return FALSE;
}

// Updates the display of the soft keyboards available to the current input method
void InputMethods::updateKeyboards(InputMethod *im )
{
    uint count;

    if ( im ) {
	QString imname = im->libName.mid(im->libName.findRev('/') + 1);

	if ( mkeyboard && !keyboardCompatible(mkeyboard, imname) ) {
	    kbdButton->setOn( FALSE );
	    showKbd( FALSE );
	    mkeyboard = 0;
	}

	count = 0;

	QValueList<InputMethod>::Iterator it;
	for ( it = inputMethodList.begin(); it != inputMethodList.end(); ++it ) {
	    if ( keyboardCompatible( &(*it), imname ) ) {
		if ( !mkeyboard ) {
		    mkeyboard = &(*it);
		    kbdButton->setPixmap( *mkeyboard->icon() );
		}

		count++;
	    }
	}
    } else {
	count = inputMethodList.count();
	if ( count && !mkeyboard ) {
	    mkeyboard = &inputMethodList[0];
	    kbdButton->setPixmap( *mkeyboard->icon() );
	} else if (!count){
	    mkeyboard = 0;  //might be redundant
	}
    }

    if ( count > 1 )
	kbdChoice->show();
    else
	kbdChoice->hide();

    if ( count )
	kbdButton->show();
    else
	kbdButton->hide();
}

void InputMethods::chooseMethod(InputMethod* im)
{
    if ( im != imethod ) {
	updateKeyboards( im );

        Config cfg("qpe");
        cfg.setGroup("InputMethod");
        if (im )
            cfg.writeEntry("im", im->name() );
        if (mkeyboard)
            cfg.writeEntry("current", mkeyboard->name() );

	QWSServer::setCurrentInputMethod( 0 );
	imethod = im;
	if ( imethod && imethod->newIM )
	    QWSServer::setCurrentInputMethod( imethod->extInterface->inputMethod() );
	else
	    QWSServer::setCurrentInputMethod( 0 );

	if ( im )
	    imButton->raiseWidget(im->widget);
	else
	    imButton->hide(); //### good UI? make sure it is shown again!
    }
}

void InputMethods::qcopReceive( const QCString &msg, const QByteArray &data )
{
    if ( imethod && imethod->newIM )
	imethod->extInterface->qcopReceive( msg, data );
}


void InputMethods::showKbd( bool on )
{
    if ( !mkeyboard )
	return;

    if ( on ) {
	mkeyboard->resetState();
	// HACK... Make the texteditor fit with all input methods
	// Input methods should also never use more than about 40% of the screen
	int height = QMIN( mkeyboard->widget->sizeHint().height(), 134 );
	#ifdef QT_QWS_SIMPAD
	mkeyboard->widget->resize( qApp->desktop()->width() / 2, height );
	#else
	mkeyboard->widget->resize( qApp->desktop()->width(), height );
	#endif
	mkeyboard->widget->move( 0, mapToGlobal( QPoint() ).y() - height );
	mkeyboard->widget->show();
    } else {
	mkeyboard->widget->hide();
    }

    emit inputToggled( on );
}

bool InputMethods::shown() const
{
    return mkeyboard && mkeyboard->widget->isVisible();
}

QString InputMethods::currentShown() const
{
    return mkeyboard && mkeyboard->widget->isVisible()
	? mkeyboard->name() : QString::null;
}

void InputMethods::sendKey( ushort unicode, ushort scancode, ushort mod, bool press, bool repeat )
{
#if defined(Q_WS_QWS)
    QWSServer::sendKeyEvent( unicode, scancode, mod, press, repeat );
#endif
}
