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

#include "wellenreiterbase.h"

#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <opie/otabwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "logwindow.h"
#include "hexwindow.h"
#include "configwindow.h"

#include <qpe/resource.h>


/* 
 *  Constructs a WellenreiterBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
WellenreiterBase::WellenreiterBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
        setName( "WellenreiterBase" );
    resize( 191, 294 ); 
    setCaption( tr( "Wellenreiter" ) );
    WellenreiterBaseLayout = new QVBoxLayout( this ); 
    WellenreiterBaseLayout->setSpacing( 2 );
    WellenreiterBaseLayout->setMargin( 0 );

    TabWidget = new OTabWidget( this, "TabWidget", OTabWidget::Global );

    ap = new QWidget( TabWidget, "ap" );
    apLayout = new QVBoxLayout( ap ); 
    apLayout->setSpacing( 2 );
    apLayout->setMargin( 2 );

    //--------- NETVIEW TAB --------------    
    
    netview = new QListView( ap, "netview" );
    netview->addColumn( tr( "SSID" ) );
    netview->setColumnAlignment( 0, AlignLeft || AlignVCenter );
    netview->addColumn( tr( "Sig" ) );
    netview->setColumnAlignment( 1, AlignCenter );
    netview->addColumn( tr( "AP" ) );
    netview->setColumnAlignment( 2, AlignCenter );
    netview->addColumn( tr( "Chn" ) );
    netview->setColumnAlignment( 3, AlignCenter );
    netview->addColumn( tr( "W" ) );
    netview->setColumnAlignment( 4, AlignCenter );
    netview->addColumn( tr( "T" ) );
    netview->setColumnAlignment( 5, AlignCenter );
    
    netview->setFrameShape( QListView::StyledPanel );
    netview->setFrameShadow( QListView::Sunken );
    netview->setRootIsDecorated( TRUE );
    apLayout->addWidget( netview );
    TabWidget->addTab( ap, "wellenreiter/networks", tr( "Networks" ) );

    //--------- LOG TAB --------------
    
    logwindow = new MLogWindow( TabWidget, "Log" );
    TabWidget->addTab( logwindow, "wellenreiter/log", tr( "Log" ) );

    //--------- HEX TAB --------------
    
    hexwindow = new MHexWindow( TabWidget, "Hex" );
    TabWidget->addTab( hexwindow, "wellenreiter/hex", tr( "Hex" ) );

    //--------- CONFIG TAB --------------
    
    configwindow = new WellenreiterConfigWindow( TabWidget, "Config" );
    TabWidget->addTab( configwindow, "wellenreiter/config", tr( "Config" ) );
    
    //--------- ABOUT TAB --------------
    
    about = new QWidget( TabWidget, "about" );
    aboutLayout = new QGridLayout( about ); 
    aboutLayout->setSpacing( 6 );
    aboutLayout->setMargin( 11 );

    PixmapLabel1_3_2 = new QLabel( about, "PixmapLabel1_3_2" );
    PixmapLabel1_3_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, PixmapLabel1_3_2->sizePolicy().hasHeightForWidth() ) );
    PixmapLabel1_3_2->setFrameShape( QLabel::Panel );
    PixmapLabel1_3_2->setFrameShadow( QLabel::Sunken );
    PixmapLabel1_3_2->setLineWidth( 2 );
    PixmapLabel1_3_2->setMargin( 0 );
    PixmapLabel1_3_2->setMidLineWidth( 0 );
    PixmapLabel1_3_2->setPixmap(  Resource::loadPixmap( "wellenreiter/logo" ) );
    PixmapLabel1_3_2->setScaledContents( TRUE );
    PixmapLabel1_3_2->setAlignment( int( QLabel::AlignCenter ) );

    aboutLayout->addWidget( PixmapLabel1_3_2, 0, 0 );

    TextLabel1_4_2 = new QLabel( about, "TextLabel1_4_2" );
    QFont TextLabel1_4_2_font(  TextLabel1_4_2->font() );
    TextLabel1_4_2_font.setFamily( "adobe-helvetica" );
    TextLabel1_4_2_font.setPointSize( 10 );
    TextLabel1_4_2->setFont( TextLabel1_4_2_font ); 
    TextLabel1_4_2->setText( tr( "<p align=center>\n"
"<hr>\n"
"Max Moser<br>\n"
"Martin J. Muench<br>\n"
"Michael Lauer<br><hr>\n"
"<b>www.remote-exploit.org</b>\n"
"</p>" ) );
    TextLabel1_4_2->setAlignment( int( QLabel::AlignCenter ) );

    aboutLayout->addWidget( TextLabel1_4_2, 1, 0 );
    TabWidget->addTab( about, "wellenreiter/about", tr( "About" ) );
    WellenreiterBaseLayout->addWidget( TabWidget );
   
    button = new QPushButton( this, "button" );
    button->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, button->sizePolicy().hasHeightForWidth() ) );
    button->setText( tr( "Start Scanning" ) );
    WellenreiterBaseLayout->addWidget( button );

    TabWidget->setCurrentTab( tr( "Networks" ) );

}

/*  
 *  Destroys the object and frees any allocated resources
 */
WellenreiterBase::~WellenreiterBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool WellenreiterBase::event( QEvent* ev )
{
    bool ret = QWidget::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	//QFont Log_2_font(  Log_2->font() );
	//Log_2_font.setFamily( "adobe-courier" );
	//Log_2_font.setPointSize( 8 );
	//Log_2->setFont( Log_2_font ); 
	QFont TextLabel1_4_2_font(  TextLabel1_4_2->font() );
	TextLabel1_4_2_font.setFamily( "adobe-helvetica" );
	TextLabel1_4_2_font.setPointSize( 10 );
	TextLabel1_4_2->setFont( TextLabel1_4_2_font ); 
    }
    return ret;
}

