/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef QDAWG_H
#define QDAWG_H

#include <qstringlist.h>

class QIODevice;
class QDawgPrivate;

class QDawg {
public:
    QDawg();
    ~QDawg();

    bool readFile(const QString&); // may mmap
    bool read(QIODevice* dev);
    bool write(QIODevice* dev) const;
    bool createFromWords(QIODevice* dev);
    void createFromWords(const QStringList&);
    QStringList allWords() const;

    bool contains(const QString&) const;
    int countWords() const;

    // Unicode 0..3FFF; an offset can be used for other ranges
    static const int nodebits=18;

    class Node {
	friend class QDawgPrivate;
	uint let:(30-nodebits);
	uint isword:1;
	uint islast:1;
	int offset:nodebits;
	Node() { }
    public:
	QChar letter() const { return QChar((ushort)let); }
	bool isWord() const { return isword; }
	bool isLast() const { return islast; }
	const Node* next() const { return islast ? 0 : this+1; }
	const Node* jump() const { return offset ? this+offset : 0; }
    };

    const Node* root() const;

    void dump() const; // debug

private:
    friend class QDawgPrivate;
    QDawgPrivate* d;
};

#endif
