/*
   This file is part of KWrite
   Copyright (c) 2000 Waldo Bastian <bastian@kde.org>
   Copyright (c) 2002 Joseph Wenninger <jowenn@kde.org>

   $Id: katebuffer.cpp,v 1.1 2002-11-10 21:10:14 jowenn Exp $

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "katebuffer.h"

// Includes for reading file
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <qfile.h>
#include <qtextstream.h>

#include <qtimer.h>
#include <qtextcodec.h>

//

#include <assert.h>
#include <kdebug.h>

/**
 * Create an empty buffer.
 */
KWBuffer::KWBuffer()
{
   clear();
}

void
KWBuffer::clear()
{
 m_stringListIt=0;
 m_stringListCurrent=0;
 m_stringList.clear();
 m_lineCount=1;
 m_stringListIt = m_stringList.append(new TextLine());
}

/**
 * Insert a file at line @p line in the buffer.
 */
void
KWBuffer::insertFile(int line, const QString &file, QTextCodec *codec)
{
	if (line) {
		qDebug("insert File only supports insertion at line 0 == file opening");
		return;
	}
	clear();
	QFile iofile(file);
	iofile.open(IO_ReadOnly);
	QTextStream stream(&iofile);
	stream.setCodec(codec);
	QString qsl;
	int count=0;
	for (count=0;((qsl=stream.readLine())!=QString::null);	count++)
	{
		if (count==0)
		{
			(*m_stringListIt)->append(qsl.unicode(),qsl.length());
		} 
		else 
		{
			TextLine::Ptr tl=new TextLine();
			tl ->append(qsl.unicode(),qsl.length());
			m_stringListIt=m_stringList.append(tl);
		}
	}
	if (count!=0) 
	{
		m_stringListCurrent=count-1;
		m_lineCount=count;
	}
}

void
KWBuffer::loadFilePart()
{
}


void
KWBuffer::insertData(int line, const QByteArray &data, QTextCodec *codec)
{
}

void
KWBuffer::slotLoadFile()
{
  loadFilePart();
//  emit linesChanged(m_totalLines);
  emit linesChanged(20);
}

/**
 * Return the total number of lines in the buffer.
 */
int
KWBuffer::count()
{
  qDebug("m_stringList.count %d",m_stringList.count());
  return m_lineCount;
//	return m_stringList.count();
//   return m_totalLines;
}


void KWBuffer::seek(int i)
{
   if (m_stringListCurrent == i)
      return;
   while(m_stringListCurrent < i)
   {
      ++m_stringListCurrent;
      ++m_stringListIt;
   }
   while(m_stringListCurrent > i)
   {
      --m_stringListCurrent;
      --m_stringListIt;
   }
}


TextLine::Ptr
KWBuffer::line(int i)
{
	if (i>=m_stringList.count()) return 0;
	seek(i);
	return *m_stringListIt;
}

void
KWBuffer::insertLine(int i, TextLine::Ptr line)
{
   seek(i);
   m_stringListIt = m_stringList.insert(m_stringListIt, line);
   m_stringListCurrent = i;
   m_lineCount++;
}


void
KWBuffer::removeLine(int i)
{
   seek(i);
   m_stringListIt = m_stringList.remove(m_stringListIt);
   m_stringListCurrent = i;
   m_lineCount--;
}

void
KWBuffer::changeLine(int i)
{
}

