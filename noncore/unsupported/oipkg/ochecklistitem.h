/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OCHECKLISTITTEM_H
#define OCHECKLISTITTEM_H

#include <qlistview.h>


class OCheckListItem : public QCheckListItem
{
public:
	OCheckListItem(OCheckListItem *parent, const char*);
	OCheckListItem(QListView *parent, const char*);
	~OCheckListItem();

  virtual void expand();

private:
	QCheckListItem *dummy;
};

#endif
