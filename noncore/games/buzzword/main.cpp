/*
 * Copyright (C) 2002 Martin Imobersteg <imm@gmx.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License,Life or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qpe/qpeapplication.h>

#include <stdlib.h>
#include <time.h>

#include "buzzword.h"

int main(int argc, char **argv)
{
	srand(time(0));

	QPEApplication a( argc, argv );
	BuzzWord *top = new BuzzWord;
	a.showMainWidget(top);

	return a.exec();
}
