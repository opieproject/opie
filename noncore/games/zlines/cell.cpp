/***************************************************************************
                          cell.cpp  -  description
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
#include "cell.h"

Cell::Cell()
{
  clear();
}

void Cell::clear()
{
  color = NOBALL;
  anim = ANIM_NO;
}

Cell& Cell::moveBall(Cell& cell)
{
	*this = cell;
  cell.clear();
  return *this;
}
Cell& Cell::operator= (const Cell& cell)
{
  color = cell.color;
  anim = cell.anim;
	return *this;
}
