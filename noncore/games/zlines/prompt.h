/***************************************************************************
                          prompt.h  -  description
                             -------------------
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PROMPT_H
#define PROMPT_H

#include <qwidget.h>
#include "ballpainter.h"

class LinesPrompt : public QWidget
{
  Q_OBJECT

  BallPainter* bPainter;
  bool PromptEnabled;
  int cb[BALLSDROP];

  void paintEvent( QPaintEvent* );
  void mousePressEvent( QMouseEvent* );

public:
  LinesPrompt( BallPainter * abPainter, QWidget * parent=0, const char * name=0 );
  ~LinesPrompt();

  void setPrompt(bool enabled);
  bool getState();  // enabled = true
  void SetBalls( int *pcb );

  int width();
  int height();
  int wPrompt();
  int hPrompt();

signals:
  void PromptPressed();

};

#endif
