/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 // (c) 2002 Patrick S. Vogtp <tille@handhelds.org>

#ifndef LISTVIEWITEMCONFIGENTRY_H
#define LISTVIEWITEMCONFIGENTRY_H

#include <qwidget.h>
#include <qlistview.h>
#include "listviewitemconffile.h"
#include "listviewitemconf.h"

class QTextStream;

class ListViewItemConfigEntry : public ListViewItemConf  {
public:
	ListViewItemConfigEntry(ListViewItemConfFile *parent, QString group, QString key="");
	~ListViewItemConfigEntry();
 	bool isGroup();
 	bool isKey();
  QString getFile();
  void setGroup(QString);
  QString getGroup();
  void setKey(QString);
  QString getKey();
  QString getValue();
	void keyChanged(QString);
	void valueChanged(QString);
  virtual void displayText();
  virtual void changed();
  virtual void remove();
  void save(QTextStream*);
  virtual void revert();
private:
 	QString _file;
	QString _group;
	QString _key;
	QString _value;
	QString _groupOrig;
	QString _keyOrig;
	QString _valueOrig;
  ListViewItemConfFile *_fileItem;
};

#endif
