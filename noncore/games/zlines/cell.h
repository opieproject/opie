/***************************************************************************
                          cell.h  -  description
                             -------------------
    begin                : Fri May 19 2000
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
#ifndef __cell_h
#define __cell_h
#include "cfg.h"


// enum int AnimType {ANIM_NO,ANIM_BORN,ANIM_JUMP,ANIM_RUN,ANIM_BURN,ANIM_BLOCK};

class Cell {
  int color;
  int anim;
public:
  Cell();

  int getColor() {return ( color );}
  int getAnim() {return ( anim );}
  bool isFree() { return (( color == NOBALL )? true:false);}

  void setColor(int c) {color = c;}
  void setAnim(int a) {anim = a;}
  void clear();
	Cell& operator = (const Cell& c);

  Cell& moveBall(Cell& cell);
};
#endif
