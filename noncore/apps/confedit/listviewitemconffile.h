/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 // (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#ifndef LISTVIEWITEMCONFFILE_H
#define LISTVIEWITEMCONFFILE_H

#include <qwidget.h>
#include <qlistview.h>
#include <qfile.h>
#include <qfileinfo.h>
#include "listviewitemconf.h"


class ListViewItemConfFile : public ListViewItemConf  {
public:
	ListViewItemConfFile(QFileInfo *file, QListView *parent=0);
	~ListViewItemConfFile();
  void parseFile();
	QString fileName();
  virtual void displayText();
  /** No descriptions */
  void save();
protected:
private:
	bool _valid;
	QFileInfo *confFileInfo;
};

#endif
