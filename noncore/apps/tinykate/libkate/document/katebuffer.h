/*
   This file is part of KWrite
   Copyright (c) 2000 Waldo Bastian <bastian@kde.org>
   Copyright (c) 2002 Joseph Wenninger <jowenn@kde.org>
   
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

#ifndef _KWBUFFER_H_
#define _KWBUFFER_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>
#include <qobject.h>
#include <qtimer.h>

#include "katetextline.h"

class QTextCodec;

/**
 * The KWBuffer class maintains a collections of lines.
 * It allows to maintain state information in a lazy way. 
 * It handles swapping out of data using secondary storage.
 *
 * It is designed to handle large amounts of text-data efficiently
 * with respect to CPU and memory usage.
 *
 * @author Waldo Bastian <bastian@kde.org>
 */
class KWBuffer : public QObject
{
   Q_OBJECT
public:
   /**
    * Create an empty buffer.
    */
   KWBuffer();

   /**
    * Insert a file at line @p line in the buffer.
    * Using @p codec to decode the file.
    */
   void insertFile(int line, const QString &file, QTextCodec *codec);

   /**
    * Insert a block of data at line @p line in the buffer.
    * Using @p codec to decode the file.
    */
   void insertData(int line, const QByteArray &data, QTextCodec *codec);

   /**
    * Return the total number of lines in the buffer.
    */
   int count();

   /**
    * Return line @p i
    */
   TextLine::Ptr line(int i);

   /**
    * Insert @p line in front of line @p i
    */
   void insertLine(int i, TextLine::Ptr line);

   /**
    * Remove line @p i
    */
   void removeLine(int i);

   /**
    * Change line @p i
    */
   void changeLine(int i);

   /**
    * Clear the buffer.
    */
   void clear();

signals:

   void textChanged();
   /**
    * Emitted during loading.
    */
   void linesChanged(int lines);
   void needHighlight(long,long);

protected:
   /**
    * Make sure @p buf gets loaded.
    */
   void loadBlock(KWBufBlock *buf);

   /**
    * Make sure @p buf gets parsed.
    */
   void parseBlock(KWBufBlock *buf);

   /**
    * Mark @p buf dirty.
    */
   void dirtyBlock(KWBufBlock *buf);
   
   /**
    * Find the block containing line @p i
    */
   KWBufBlock *findBlock(int i);

   /**
    * Load a part of the file that is currently loading.
    */
   void loadFilePart();
   
protected slots:
   void slotLoadFile();
   
protected:
   TextLine::List m_stringList;
   TextLine::List::Iterator m_stringListIt;
   int m_stringListCurrent;
   int m_lineCount;
   void seek(int i);
   

};

#endif
