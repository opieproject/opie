/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: iplined.h,v 1.2 2003-08-09 17:14:55 kergoth Exp $
 * 
 *            Copyright (C) 1997 Bernd Johannes Wuebben 
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _IPLINED_H_
#define _IPLINED_H_

#include <qlineedit.h>

class IPLineEdit : public QLineEdit {
public:
  IPLineEdit( QWidget *parent=0, const char *name=0 );
  ~IPLineEdit() {}

  virtual QSize sizeHint() const;
};

#endif
