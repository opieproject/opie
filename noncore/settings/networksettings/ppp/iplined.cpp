/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: iplined.cpp,v 1.1 2003-05-23 19:43:46 tille Exp $
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

#include "iplined.h"

IPLineEdit::IPLineEdit( QWidget *parent, const char *name )
    : QLineEdit(parent, name )//, "0123456789.")
{
  setMaxLength(3 * 4 + 1 * 3);
}

QSize IPLineEdit::sizeHint() const {
  QFontMetrics fm = fontMetrics();

  QSize s;
  s.setHeight(QLineEdit::sizeHint().height());
  s.setWidth(fm.boundingRect("888.888.888.888XX").width());
  return s;
}
