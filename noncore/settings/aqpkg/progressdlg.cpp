/***************************************************************************
                          progressdlg.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "progressdlg.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "global.h"
/* 
 *  Constructs a ProgressDlg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ProgressDlg::ProgressDlg( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
    	setName( "ProgressDlg" );
    resize( 240, 73 ); 
    setCaption( tr( "Progress" ) );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 10, 10, 240, 50 ) ); 
    TextLabel2->setFrameShape( QLabel::Box );
    TextLabel2->setText( tr( "Text" ) );
    TextLabel2->setAlignment( QLabel::WordBreak | QLabel::AlignHCenter | QLabel::AlignVCenter );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ProgressDlg::~ProgressDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void ProgressDlg :: setText( QString &text )
{
    TextLabel2->setText( text );
}

void ProgressDlg :: setText( const char *text )
{
    TextLabel2->setText( text );
}
