/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/inputmethodinterface.h>
#include <qpe/opielibrary.h>
#include <qpe/global.h>

#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qdir.h>
#include <stdlib.h>
#include <qtranslator.h>

#ifdef Q_WS_QWS
#include <qwindowsystem_qws.h>
#include <qwsevent_qws.h>
#endif

#ifdef SINGLE_APP
#include "handwritingimpl.h"
#include "keyboardimpl.h"
#include "pickboardimpl.h"
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

InputMethods::InputMethods( QWidget *parent ) :
    QWidget( parent, "InputMethods", WStyle_Tool | WStyle_Customize )
{
    method = NULL;

    QHBoxLayout *hbox = new QHBoxLayout( this );

    kbdButton = new QToolButton( this );
    kbdButton->setFocusPolicy(NoFocus);
    kbdButton->setToggleButton( TRUE );
    kbdButton->setFixedHeight( 17 );
    kbdButton->setFixedWidth( 32 );
    kbdButton->setAutoRaise( TRUE );
    kbdButton->setUsesBigPixmap( TRUE );
    hbox->addWidget( kbdButton );
    connect( kbdButton, SIGNAL(toggled(bool)), this, SLOT(showKbd(bool)) );

    kbdChoice = new QToolButton( this );
    kbdChoice->setFocusPolicy(NoFocus);
    kbdChoice->setPixmap( QPixmap( (const char **)tri_xpm ) );
    kbdChoice->setFixedHeight( 17 );
    kbdChoice->setFixedWidth( 12 );
    kbdChoice->setAutoRaise( TRUE );
    hbox->addWidget( kbdChoice );
    connect( kbdChoice, SIGNAL(clicked()), this, SLOT(chooseKbd()) );

    connect( (QPEApplication*)qApp, SIGNAL(clientMoused()),
      this, SLOT(resetStates()) );

    loadInputMethods();
}

InputMethods::~InputMethods()
{
#ifndef SINGLE_APP
    QValueList<InputMethod>::Iterator mit;
    for ( mit = inputMethodList.begin(); mit != inputMethodList.end(); ++mit ) {
  int i = (*mit).interface->release();
  (*mit).library->unload();
  delete (*mit).library;
    }
#endif
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
  if ( (*it).interface->name() == name ) {
      im = &(*it);
      break;
  }
    }
    if ( im )
  chooseMethod(im);
}

void InputMethods::resetStates()
{
    if ( method )
  method->interface->resetState();
}

QRect InputMethods::inputRect() const
{
    if ( !method || !method->widget->isVisible() )
  return QRect();
    else
  return method->widget->geometry();
}

void InputMethods::loadInputMethods()
{
#ifndef SINGLE_APP
    hideInputMethod();
    method = 0;

    QValueList<InputMethod>::Iterator mit;
    for ( mit = inputMethodList.begin(); mit != inputMethodList.end(); ++mit ) {
  (*mit).interface->release();
  (*mit).library->unload();
  delete (*mit).library;
    }
    inputMethodList.clear();

    QString path = QPEApplication::qpeDir() + "/plugins/inputmethods";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
  InputMethodInterface *iface = 0;
  OpieLibrary *lib = new OpieLibrary( path + "/" + *it );
  if ( lib->queryInterface( IID_InputMethod, (QUnknownInterface**)&iface ) == QS_OK ) {
      InputMethod input;
      input.library = lib;
      input.interface = iface;
      input.widget = input.interface->inputMethod( 0, inputWidgetStyle );
      input.interface->onKeyPress( this, SLOT(sendKey(ushort,ushort,ushort,bool,bool)) );
      inputMethodList.append( input );

      QString type = (*it).left( (*it).find(".") );
      QStringList langs = Global::languageList();
      for (QStringList::ConstIterator lit = langs.begin(); lit!=langs.end(); ++lit) {
    QString lang = *lit;
    QTranslator * trans = new QTranslator(qApp);
    QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";
    if ( trans->load( tfn ))
        qApp->installTranslator( trans );
    else
        delete trans;
      }
  } else {
      delete lib;
  }
    }
#else
    InputMethod input;
    input.interface = new HandwritingImpl();
    input.widget = input.interface->inputMethod( 0, inputWidgetStyle );
    input.interface->onKeyPress( this, SLOT(sendKey(ushort,ushort,ushort,bool,bool)) );
    inputMethodList.append( input );
    input.interface = new KeyboardImpl();
    input.widget = input.interface->inputMethod( 0, inputWidgetStyle );
    input.interface->onKeyPress( this, SLOT(sendKey(ushort,ushort,ushort,bool,bool)) );
    inputMethodList.append( input );
    input.interface = new PickboardImpl();
    input.widget = input.interface->inputMethod( 0, inputWidgetStyle );
    input.interface->onKeyPress( this, SLOT(sendKey(ushort,ushort,ushort,bool,bool)) );
    inputMethodList.append( input );
#endif
    if ( !inputMethodList.isEmpty() ) {
        Config cfg("qpe");
        cfg.setGroup("InputMethod");
        QString curMethod = cfg.readEntry("current","");
        if(curMethod.isEmpty()) {
            method = &inputMethodList[0];
        } else {
            int i = 0;
            QValueList<InputMethod>::Iterator it;
            for ( it = inputMethodList.begin(); it != inputMethodList.end(); ++it, i++ ) {
                if((*it).interface->name() == curMethod) {
                    method = &inputMethodList[i];
//                   qDebug(curMethod);
                }
            }
        }
        kbdButton->setPixmap( *method->interface->icon() );
    }
    if ( !inputMethodList.isEmpty() )
  kbdButton->show();
    else
  kbdButton->hide();
    if ( inputMethodList.count() > 1 )
  kbdChoice->show();
    else
  kbdChoice->hide();
}

void InputMethods::chooseKbd()
{
    QPopupMenu pop( this );

    int i = 0;
    QValueList<InputMethod>::Iterator it;
    for ( it = inputMethodList.begin(); it != inputMethodList.end(); ++it, i++ ) {
  pop.insertItem( (*it).interface->name(), i );
  if ( method == &(*it) )
      pop.setItemChecked( i, TRUE );
    }

    QPoint pt = mapToGlobal(kbdChoice->geometry().topRight());
    QSize s = pop.sizeHint();
    pt.ry() -= s.height();
    pt.rx() -= s.width();
    i = pop.exec( pt );
    if ( i == -1 )
  return;
    InputMethod *im = &inputMethodList[i];
    chooseMethod(im);
}

void InputMethods::chooseMethod(InputMethod* im)
{
    if ( im != method ) {
        if ( method && method->widget->isVisible() )
            method->widget->hide();
        method = im;
        Config cfg("qpe");
        cfg.setGroup("InputMethod");
        cfg.writeEntry("current",  method->interface->name());
        kbdButton->setPixmap( *method->interface->icon() );
    }
    if ( !kbdButton->isOn() )
  kbdButton->setOn( TRUE );
    else
  showKbd( TRUE );
}


void InputMethods::showKbd( bool on )
{
    if ( !method )
  return;

    if ( on ) {
  method->interface->resetState();
  // HACK... Make the texteditor fit with all input methods
  // Input methods should also never use more than about 40% of the screen
  int height = QMIN( method->widget->sizeHint().height(), 134 );
  method->widget->resize( qApp->desktop()->width(), height );
  method->widget->move( 0, mapToGlobal( QPoint() ).y() - height );
  method->widget->show();
    } else {
  method->widget->hide();
    }

    emit inputToggled( on );
}

bool InputMethods::shown() const
{
    return method && method->widget->isVisible();
}

QString InputMethods::currentShown() const
{
    return method && method->widget->isVisible()
  ? method->interface->name() : QString::null;
}

void InputMethods::sendKey( ushort unicode, ushort scancode, ushort mod, bool press, bool repeat )
{
#if defined(Q_WS_QWS)
    QWSServer::sendKeyEvent( unicode, scancode, mod, press, repeat );
#endif
}
