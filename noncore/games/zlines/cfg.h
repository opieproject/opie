/***************************************************************************
                          cfg.h  -  description
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
#ifndef __CFG_H
#define __CFG_H 1

#define LINESVERSION "0.6.1"

#define NCOLORS 7
#define BALLSDROP 3

#define NOBALL -1
#define PIXTIME 10
#define FIREBALLS 4
#define BOOMBALLS 4
#define FIREPIX 5
#define STEPTIME 3
#define NORMALBALL PIXTIME + BOOMBALLS
#define TIMERCLOCK 25

enum Anim
{
  ANIM_NO,
  ANIM_JUMP,
  ANIM_RUN,
  ANIM_BORN,
  ANIM_BURN,
  ANIM_YES,
  ANIM_BLOCK
};

//#define _DBG_

#endif //__CFG_H
