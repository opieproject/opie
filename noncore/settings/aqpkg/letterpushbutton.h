/***************************************************************************
                          letterpushbutton.h  -  description
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

#ifndef LETTERPUSHBUTTON_H
#define LETTERPUSHBUTTON_H

#include <qpushbutton.h>

/**
  *@author Andy Qua
  */

class LetterPushButton : public QPushButton
{
    Q_OBJECT
public:
    LetterPushButton( const QString & text, QWidget * parent, const char * name=0 );
    ~LetterPushButton();

public slots:
    void released_emmitor();

signals:
    void released( QString text );
};

#endif
