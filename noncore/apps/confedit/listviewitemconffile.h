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
	QString fileName();
  void save();
  virtual void displayText();
  virtual bool revertable();
  virtual void revert();
  virtual void remove();
  virtual void expand();
  QString backupFileName();
  void parseFile();
protected:
private:
	bool _valid;
	QFileInfo *confFileInfo;
};

#endif
