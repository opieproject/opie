/***************************************************************************
                          multiline_ex.cpp  -  description
                             -------------------
    begin                : Fri Aug 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//#include <qmultilineedit.h>
#include "multiline_ex.h"

struct MultiLine_ExData
//class  MultiLine_ExData
{
public:
    QColorGroup mypapcolgrp;
    QColorGroup papcolgrp;
    uint ownpalette : 1;
};


MultiLine_Ex::MultiLine_Ex( QWidget *parent ,  const char *name  )
    :QMultiLineEdit( parent,name )
{

#ifndef Q_WS_QWS
//    clearTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar);  //for pre release testing
#else
// clearTableFlags( Tbl_autoVScrollBar );  //for pre release testing
#endif
//     clearTableFlags( Tbl_autoHScrollBar | Tbl_autoVScrollBar); // for release
//    init();
}

void MultiLine_Ex::pageDown( bool mark )
{
  QMultiLineEdit::pageDown( mark );
}

void MultiLine_Ex::pageUp( bool mark )
{
  QMultiLineEdit::pageUp(  mark );
}


void MultiLine_Ex::cursorLeft( bool mark, bool wrap )
{
  QMultiLineEdit::cursorLeft( mark, wrap );
}

void MultiLine_Ex::cursorRight( bool mark, bool wrap )
{
  QMultiLineEdit::cursorRight(  mark,  wrap );
}

void MultiLine_Ex::cursorUp( bool mark )
{
  QMultiLineEdit::cursorUp(  mark );
}

void MultiLine_Ex::cursorDown( bool mark )
{
  QMultiLineEdit::cursorDown(  mark );
}


//void MultiLine_Ex::setPaper( const QBrush& pap)
//{

//}

int MultiLine_Ex::lastRow()
{
  return lastRowVisible();
}

int MultiLine_Ex::topRow()
{
  return topCell();
}

int MultiLine_Ex::editSize( )
{
  return viewHeight() / cellHeight();
//scroll( 0, int yPixels );
}

void MultiLine_Ex::ScrollUp( int lines )
{
    for( int i = 0; i < lines; i++) {
    this->setTopCell( topCell() + 1 );
    }
}

void MultiLine_Ex::ScrollDown( int lines )
{
  for( int i = 0; i < lines; i++) {
  this->setTopCell( topCell() -1 );
  }
}

int MultiLine_Ex::lineHeight( int row)
{
  return this->cellHeight(row );
}

int MultiLine_Ex::Top( )
{
  return this->topCell();
}

int MultiLine_Ex::Rows( )
{
  return findRow ( 1 );
}

int MultiLine_Ex::PageSize()
{
  return viewHeight() / cellHeight() ;
}

bool MultiLine_Ex::hasSelectedText()
{
  return QMultiLineEdit::hasMarkedText();
}
