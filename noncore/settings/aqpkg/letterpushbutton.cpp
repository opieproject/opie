/***************************************************************************
                          letterpushbutton.cpp  -  description
                             -------------------
    begin                : Wed Oct 16 2002
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

#include "letterpushbutton.h"

LetterPushButton :: LetterPushButton( const QString &text, QWidget *parent, const char *name )
   : QPushButton( text, parent, name )
{
   connect( this, SIGNAL(released()), this, SLOT(released_emmitor()) );
}

LetterPushButton :: ~LetterPushButton()
{
}

void LetterPushButton :: released_emmitor()
{
    emit released( text() );
}

