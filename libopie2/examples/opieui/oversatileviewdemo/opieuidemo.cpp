/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
***********************************************************************/

// Qt

#include <qcolor.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qvbox.h>
#include <qstring.h>
#include <qstringlist.h>

// Qtopia

#include <qpe/qpeapplication.h>
#include <qpe/global.h>

// Opie

#include <opie2/odevice.h>

#include <opie2/ocompletionbox.h>
#include <opie2/olineedit.h>
#include <opie2/ocombobox.h>
#include <opie2/oeditlistbox.h>
#include <opie2/oselector.h>
#include <opie2/opopupmenu.h>

#include <qtabwidget.h>
#include "oversatileviewdemo.h"

// Local

#include "opieuidemo.h"

using namespace Opie::Core;
using namespace Opie::Ui;

enum Demos { ocompletionbox, olineedit, ocombobox, oeditlistbox, oselector };

OpieUIDemo::OpieUIDemo( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{

    QMenuBar* mbar = this->menuBar();
    OPopupMenu* demo = new OPopupMenu( this );
    demo->setTitle( "Title" );
    demo->setItemParameter( demo->insertItem( "OCompletionBox", this, SLOT( demo(int) ) ), ocompletionbox );
    demo->setItemParameter( demo->insertItem( "OLineEdit", this, SLOT( demo(int) ) ), olineedit );
    demo->setItemParameter( demo->insertItem( "OComboBox", this, SLOT( demo(int) ) ), ocombobox );
    demo->setItemParameter( demo->insertItem( "OEditListBox", this, SLOT( demo(int) ) ), oeditlistbox );
    demo->setItemParameter( demo->insertItem( "OSelector", this, SLOT( demo(int) ) ), oselector );
    mbar->insertItem( "Demonstrate", demo );

    build();

}

OpieUIDemo::~OpieUIDemo()
{
}

void OpieUIDemo::build()
{
    main = new QTabWidget( this, "tabwidget" );
    setCentralWidget( main );
    main->show();

    main->addTab( new OVersatileViewDemo( main ), "VersatileView" );
}


void OpieUIDemo::demo( int d )
{
    switch (d)
    {
        case ocompletionbox: demoOCompletionBox(); break;
        case olineedit: demoOLineEdit(); break;
        case ocombobox: demoOComboBox(); break;
        case oeditlistbox: demoOEditListBox(); break;
        case oselector: demoOSelector(); break;

    }

}

void OpieUIDemo::demoOCompletionBox()
{
    qDebug( "ocompletionbox" );

    OCompletionBox* box = new OCompletionBox( 0 );
    box->insertItem( "This CompletionBox" );
    box->insertItem( "Says 'Hello World'" );
    box->insertItem( "Here are some" );
    box->insertItem( "Additional Items" );
    box->insertItem( "Complete Completion Box" );

    connect( box, SIGNAL( activated(const QString&) ), this, SLOT( messageBox(const QString&) ) );
    box->popup();

}

void OpieUIDemo::demoOLineEdit()
{
    qDebug( "olineedit" );

    OLineEdit *edit = new OLineEdit( 0, "lineedit" );

    edit->setCompletionMode( OGlobalSettings::CompletionPopup );
    OCompletion* comp = edit->completionObject();

    QStringList list;
    list << "mickeyl@handhelds.org";
    list << "mickey@tm.informatik.uni-frankfurt.de";
    list << "mickey@vanille.de";

    comp->setItems( list );

    edit->show();

}

void OpieUIDemo::demoOComboBox()
{
    qDebug( "ocombobox" );

    OComboBox *combo = new OComboBox( true, 0, "combobox" );

    combo->setCompletionMode( OGlobalSettings::CompletionPopup );
    OCompletion* comp = combo->completionObject();

    QStringList ilist;
    ilist << "kergoth@handhelds.org";
    ilist << "harlekin@handhelds.org";
    ilist << "groucho@handhelds.org";
    combo->insertStringList( ilist );

    QStringList clist;
    clist << "mickeyl@handhelds.org";
    clist << "mickey@tm.informatik.uni-frankfurt.de";
    clist << "mickey@vanille.de";
    comp->setItems( clist );

    combo->show();

}

void OpieUIDemo::demoOEditListBox()
{
    qDebug( "oeditlistbox" );

    OEditListBox* edit = new OEditListBox( "OEditListBox", 0, "editlistbox" );

    edit->lineEdit()->setCompletionMode( OGlobalSettings::CompletionPopup );
    OCompletion* comp = edit->lineEdit()->completionObject();
    QStringList clist;
    clist << "Completion everywhere";
    clist << "Cool Completion everywhere";
    clist << "History History History";
    comp->setItems( clist );

    QStringList list;
    list << "kergoth@handhelds.org";
    list << "harlekin@handhelds.org";
    list << "groucho@handhelds.org";
    list << "mickeyl@handhelds.org";
    edit->insertStringList( list );

    edit->show();

}

void OpieUIDemo::demoOSelector()
{
    qDebug( "oselector" );

    OHSSelector* sel = new OHSSelector( 0, "gradientselector" );
    //#sel->resize( QSize( 200, 30 ) );
    //#sel->setColors( QColor( 90, 190, 60 ), QColor( 200, 55, 255 ) );
    //#sel->setText( "Dark", "Light" );

    sel->show();
}

void OpieUIDemo::messageBox( const QString& text )
{
    QString info;
    info = "You have selected '" + text + "'";
    QMessageBox::information( this, "OpieUIDemo", info );
}
