/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
  // (c) 2002 Patrick S. Vogtp <tille@handhelds.org>

#include "listviewitemconfigentry.h"
#include "listviewitemconffile.h"
#include <qtextstream.h>

ListViewItemConfigEntry::ListViewItemConfigEntry(ListViewItemConfFile *parent, QString group, QString key)
   : ListViewItemConf(parent)
{
	_fileItem = parent;
	_file = parent->fileName();
 	_group = group;
	setKey(key);
 	_fileItem->unchanged();
}

ListViewItemConfigEntry::~ListViewItemConfigEntry()
{
}

bool ListViewItemConfigEntry::isGroup()
{
 	return _key.isEmpty();
}

bool ListViewItemConfigEntry::isKey()
{
 	return !_key.isEmpty();
}

QString ListViewItemConfigEntry::getFile()
{
 	return _file;
}

void ListViewItemConfigEntry::setGroup(QString g)
{
	if (g==_group) return;
 	_group = g;
  changed();
}

QString ListViewItemConfigEntry::getGroup()
{
 	return _group;
}

void ListViewItemConfigEntry::setKey(QString key)
{
 	int pos = key.find("=");
  _key = key.left(pos).stripWhiteSpace();
  _value = key.right(key.length() - pos - 1).stripWhiteSpace();
 	displayText();
}

QString ListViewItemConfigEntry::getKey()
{
 	return _key;
}

QString ListViewItemConfigEntry::getValue()
{
 	return _value;
}

void ListViewItemConfigEntry::keyChanged(QString k)
{
	if (k==_key) return;
	_key = k;
 	changed();
}

void ListViewItemConfigEntry::valueChanged(QString v)
{
	if (v==_value) return;
	_value = v;
 	changed();
}

void ListViewItemConfigEntry::displayText()
{
	QString s;
 	if (_changed) s="*";
  if (isGroup())
  {
	  s += "["+_group+"]";
   	_type = Group;
  }else{
	  s += _key+" = "+_value;
   _type = Key;
  }
	setText(0,s);
}

void ListViewItemConfigEntry::changed()
{
	_changed=true;
 	displayText();
 	_fileItem->changed();
}

void ListViewItemConfigEntry::save(QTextStream *t)
{
	QString s;
  if (isGroup())
  {
	  s += "["+_group+"]";
   	_type = Group;
  }else{
	  s += _key+" = "+_value;
   _type = Key;
  }
  s += "\n";
	(*t) << s;
 	_changed = false;
 	for (QListViewItem *it = firstChild(); it!=0;it = it->nextSibling())
  {
   	((ListViewItemConfigEntry*)it)->save(t);
  }
}