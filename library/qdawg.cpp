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
#include "qdawg.h"
#include <qintdict.h>
#include <qvaluelist.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qtl.h>

#include <limits.h>
#include <stdio.h>

// for mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

class QDawgPrivate;
class QTrie;

typedef QValueList<QTrie*> TrieClub;
typedef QIntDict<TrieClub> TrieClubDirectory;

class TriePtr {
public:
    QChar letter;
    QTrie* p;
    int operator <(const TriePtr& o) const;
    int operator >(const TriePtr& o) const;
    int operator <=(const TriePtr& o) const;
};

class TrieList : public QValueList<TriePtr> {
    bool sorted;
public:
    TrieList()
    {
	sorted=TRUE;
    }

    QTrie* findAdd(QChar c);
    bool equal(TrieList& l);

    void sort()
    {
	if ( !sorted ) {
	    qHeapSort(*this);
	    sorted = TRUE;
	}
    }
};

// A fast but memory-wasting temporary class.  The Dawg is the goal.
class QTrie {
public:
    QTrie();
    ~QTrie();

    void insertWord(const QString& s, uint index=0);
    bool equal(QTrie* o);
    void dump(int indent=0);

private:
    TrieList children;
    bool isword;

    friend class QDawgPrivate;
    int maxdepth;
    int decendants;
    int key;
    void distributeKeys(TrieClubDirectory& directory);
    QTrie* clubLeader(TrieClubDirectory& directory);
    int collectKeys();
    friend class TriePtr;
    friend class TrieList;
};

QTrie::QTrie()
{
    key = 0;
    isword = FALSE;
}

QTrie::~QTrie()
{
    // NOTE: we do not delete the children - after conversion to DAWG
    // it's too difficult.  The QTrie's are deleted via the directory.
}

void QTrie::insertWord(const QString& s, uint index)
{
    if ( index == s.length() ) {
	isword = TRUE;
    } else {
	QTrie* t = children.findAdd(s[index]);
	t->insertWord(s,index+1);
    }
}

bool QTrie::equal(QTrie* o)
{
    if ( o == this ) return TRUE;
    if ( isword != o->isword )
	return FALSE;
    return children.equal(o->children);
}

void QTrie::dump(int indent)
{
    for (TrieList::Iterator it=children.begin(); it!=children.end(); ++it) {
	QTrie* s = (*it).p;
	for (int in=0; in<indent; in++)
	    fputc(' ',stderr);
	fprintf(stderr," %c %d %s %p\n",(*it).letter.unicode(),
	    s->key,s->isword?"word":"",s);
	s->dump(indent+2);
    }
}

void QTrie::distributeKeys(TrieClubDirectory& directory)
{
    maxdepth = INT_MIN;
    decendants = children.count();
    key = 0;
    for (TrieList::Iterator it=children.begin(); it!=children.end(); ++it) {
	QTrie* s = (*it).p;
	QChar l = (*it).letter;
	s->distributeKeys(directory);
	key = key*64+l.unicode()+s->key*5;
	decendants += s->decendants;
	if ( s->maxdepth+1 > maxdepth )
	    maxdepth = s->maxdepth+1;
    }
    if ( decendants ) {
	key += decendants + maxdepth*256 + children.count() * 65536;
	if ( !key ) key++; // unlikely
    }
    TrieClub* c = directory[key];
    if ( !c ) directory.insert(key, (c = new TrieClub) );
    c->prepend(this);
}

QTrie* QTrie::clubLeader(TrieClubDirectory& directory)
{
    if ( !key ) return directory[0]->first();
    for (TrieList::Iterator it=children.begin(); it!=children.end(); ++it) {
	QTrie* t= (*it).p->clubLeader(directory);
	(*it).p = t;
    }
    TrieClub *club = directory[key];
    for (TrieClub::Iterator it = club->begin(); it != club->end(); ++it) {
	QTrie* o = *it;
	if ( o->equal(this) )
	    return o;
    }
    return this;
}

int QTrie::collectKeys()
{
    int n=0;
    if ( key ) key=0,n+=children.count();
    for (TrieList::Iterator it=children.begin(); it!=children.end(); ++it)
	n += (*it).p->collectKeys();
    return n;
}

int TriePtr::operator <(const TriePtr& o) const
    { return letter < o.letter; }
int TriePtr::operator >(const TriePtr& o) const
    { return letter > o.letter; }
int TriePtr::operator <=(const TriePtr& o) const
    { return letter <= o.letter; }

bool TrieList::equal(TrieList& l)
{
    if ( count() != l.count() )
	return FALSE;
    sort(); l.sort();
    ConstIterator it2 = begin();
    ConstIterator it = l.begin();
    for( ; it != l.end(); ++it, ++it2 )
	if ( (*it).letter != (*it2).letter || ! (*it).p->equal((*it2).p) )
	    return FALSE;
    return TRUE;
}
QTrie* TrieList::findAdd(QChar c)
{
    for (Iterator it=begin(); it!=end(); ++it) {
	if ( (*it).letter == c )
	    return (*it).p;
    }
    TriePtr p;
    p.p = new QTrie;
    p.letter = c;
    prepend(p);
    sorted=FALSE;
    sort();
    return p.p;
}

static const char* dawg_sig = "QDAWG100";

class QDawgPrivate {
public:
    QDawgPrivate(QIODevice* dev)
    {
	QDataStream ds(dev);
	char sig[8];
	ds.readRawBytes(sig,8);
	if ( !strncmp(dawg_sig,sig,8) ) {
	    uint n;
	    char* nn;
	    ds.readBytes(nn,n);

	    // #### endianness problem ignored.
	    node = (QDawg::Node*)nn;
	    nodes = n / sizeof(QDawg::Node);
	} else {
	    node = 0;
	}
    }

    bool ok() const { return node; }

    QDawgPrivate(uchar* mem)
    {
	if ( !strncmp(dawg_sig,(char*)mem,8) ) {
	    mem += 8;

	    int n = ((mem[0]*256+mem[1])*256+mem[2])*256+mem[3];
	    mem += 4;

	    // #### endianness problem ignored.
	    node = (QDawg::Node*)((char*)mem);
	    nodes = n / sizeof(QDawg::Node);
	}
    }

    QDawgPrivate(QTrie* t) // destroys the QTrie.
    {
	TrieClubDirectory directory(9973);
	t->distributeKeys(directory);
	QTrie* l = t->clubLeader(directory);
	ASSERT(l==t);
	generateArray(t);

	TrieClub *club;
	for (QIntDictIterator<TrieClub> dit(directory); (club=dit); ++dit)
	{
	    for (TrieClub::Iterator it = club->begin(); it != club->end(); ++it) {
		delete *it;
	    }
	    delete club;
	}
    }

    bool write(QIODevice* dev)
    {
	QDataStream ds(dev);
	ds.writeRawBytes(dawg_sig,8);
	// #### endianness problem ignored.
	ds.writeBytes((char*)node,sizeof(QDawg::Node)*nodes);
	return dev->state() == IO_Ok;
    }

    void dumpWords(int nid=0, int index=0)
    {
	static char word[256]; // ick latin1
	int i=0;
	do {
	    QDawg::Node& n = node[nid+i];
	    word[index] = n.let;
	    if ( n.isword )
		fprintf(stderr,"%.*s\n",index+1,word);
	    if ( n.offset ) dumpWords(n.offset+nid+i,index+1);
	} while (!node[nid+i++].islast);
    }

    void dump(int nid=0, int indent=0)
    {
	int i=0;
	do {
	    QDawg::Node& n = node[nid+i];
	    fprintf(stderr,"%d: ",nid+i);
	    for (int in=0; in<indent; in++)
		fputc(' ',stderr);
	    fprintf(stderr," %c %d %d %d\n",n.let,
		n.isword,n.islast,n.offset);
	    if ( n.offset ) dump(n.offset+nid+i,indent+2);
	} while (!node[nid+i++].islast);
    }

    int countWords(int nid=0)
    {
	int t=0;
	int i=0;
	do {
	    QDawg::Node& n = node[nid+i];
	    if ( n.isword )
		t++;
	    if ( n.offset )
		t+=countWords(n.offset+nid+i);
	} while (!node[nid+i++].islast);
	return t;
    }

    bool contains(const QString& s, int nid=0, int index=0) const
    {
	int i=0;
	do {
	    QDawg::Node& n = node[nid+i];
	    if ( s[index] == QChar((ushort)n.let) ) {
		if ( n.isword && index == (int)s.length()-1 )
		    return TRUE;
		if ( n.offset )
		    return contains(s,n.offset+nid+i,index+1);
	    }
	} while (!node[nid+i++].islast);
	return FALSE;
    }

    void appendAllWords(QStringList& list, int nid=0, QString s="") const
    {
	int i=0;
	int next = s.length();
	do {
	    QDawg::Node& n = node[nid+i];
	    s[next] = QChar((ushort)n.let);
	    if ( n.isword )
		list.append(s);
	    if ( n.offset )
		appendAllWords(list, n.offset+nid+i, s);
	} while (!node[nid+i++].islast);
    }

    const QDawg::Node* root() { return node; }

private:
    void generateArray(QTrie* t)
    {
	nodes = 0;
	int n = t->collectKeys();
	node = new QDawg::Node[n];
	appendToArray(t);
	ASSERT(n == nodes);
    }

    int appendToArray(QTrie* t)
    {
	if ( !t->key ) {
	    if ( !t->children.count() )
		return 0;
	    t->key = nodes;
	    nodes += t->children.count();
	    QDawg::Node* n = &node[t->key-1];
	    int here = t->key;
	    for (TrieList::Iterator it=t->children.begin(); it!=t->children.end(); ++it) {
		QTrie* s = (*it).p;
		++n;
		n->let = (*it).letter.unicode();
		n->isword = s->isword;
		n->islast = 0;
		n->offset = appendToArray(s);
		if ( n->offset ) {
		    int t = n->offset-here;
		    n->offset=t;
		    if ( n->offset != t )
			qWarning("Overflow: too many words");
		}
		here++;
	    }
	    n->islast = 1;
	}
	return t->key;
    }

private:
    int nodes;
    QDawg::Node *node;
};

/*!
  \class QDawg qdawg.h
  \brief The QDawg class provides an implementation of a Directed Acyclic Word Graph.

  A DAWG provides very fast look-up of words in a word list.

  The word list is created using readFile(), read() or
  createFromWords(). A list of all the DAWG's words is returned by
  allWords(), and the total number of words is returned by
  countWords(). Use contains() to see if a particular word is in the
  DAWG. The root \link qdawg-node.html node\endlink is returned by root().

  A global DAWG is maintained for the current locale. See the
  \l Global class for details.

  The structure of a DAWG is a graph of \link qdawg-node.html
  Nodes\endlink. There are no cycles in the graph (since there are no
  inifinitely repeating words). Each \link qdawg-node.html
  Node\endlink is a member of a list of \link qdawg-node.html
  Nodes\endlink called a child list. Each \link qdawg-node.html
  Node\endlink in the child list has a \e letter, an \e isWord flag,
  at most one \e jump arc, and at most one arc to the next child in
  the list.

  If you traverse the \link qdawg-node.html Nodes\endlink in a DAWG,
  starting from the root(), and you concatenate all the letters from
  the single child in each child list that you visit, at every \link
  qdawg-node.html Node\endlink which has the isWord flag set your
  concatenation will be a word in the list represented by the DAWG.

  For example, the DAWG below represents the word list:
  ban, band, can, cane, cans, pan, pane, pans.

  This structuring not only provides O(1) lookup of words in the word list,
  but also produces a smaller storage file than a plain text file word list.

  \img qdawg.png
*/

/*!
    Constructs a new empty DAWG.
*/
QDawg::QDawg()
{
    d = 0;
}

/*!
    Deletes the DAWG.
*/
QDawg::~QDawg()
{
    delete d;
}

/*!
  \overload
  Replaces all the DAWG's words with words read from \a dev.
*/
bool QDawg::createFromWords(QIODevice* dev)
{
    delete d;

    QTextStream i(dev);
    QTrie* trie = new QTrie;
    int n=0;
    while (!i.atEnd()) {
	trie->insertWord(QString::fromUtf8(i.readLine()));
	n++;
    }
    if ( n )
	d = new QDawgPrivate(trie);
    else
	d = 0;
    return TRUE;
}

/*!
  Replaces all the DAWG's words with the words in the \a list.
*/
void QDawg::createFromWords(const QStringList& list)
{
    delete d;

    if ( list.count() ) {
	QTrie* trie = new QTrie;
	for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it) {
	    trie->insertWord(*it);
	}
	d = new QDawgPrivate(trie);
    } else {
	d = 0;
    }
}

/*!
  Returns a list of all the words in the DAWG.
*/
QStringList QDawg::allWords() const
{
    QStringList result;
    if ( d ) d->appendAllWords(result);
    return result;
}


/*!
  Replaces the DAWG with the DAWG in \a filename.
  The file is memory-mapped.

  \sa write()
*/
bool QDawg::readFile(const QString& filename)
{
    delete d;
    d = 0;
    int f = ::open( QFile::encodeName(filename), O_RDONLY );
    if ( f < 0 )
	return FALSE;
    struct stat st;
    if ( !fstat( f, &st ) ) {
	char * tmp = (char*)mmap( 0, st.st_size, // any address, whole file
                       PROT_READ, // read-only memory
                       MAP_FILE | MAP_PRIVATE, // swap-backed map from file
                       f, 0 ); // from offset 0 of f
	if ( tmp && tmp != (char*)MAP_FAILED )
	    d = new QDawgPrivate((uchar*)tmp);
    }
    ::close( f );
    return d;
}

/*!
  Replaces the DAWG with the DAWG in \a dev.
  The file is memory-mapped.

  \sa write()
*/
bool QDawg::read(QIODevice* dev)
{
    delete d;
    d = new QDawgPrivate(dev);
    if ( d->ok() )
	return TRUE;
    delete d;
    d = 0;
    return FALSE;
}

/*!
  Writes the DAWG to \a dev, in a custom QDAWG format.
*/
bool QDawg::write(QIODevice* dev) const
{
    return d ? d->write(dev) : TRUE;
}

/*!
  Returns the number of words in the DAWG.
*/
int QDawg::countWords() const
{
    return d ? d->countWords() : 0;
}

/*!
  Returns the root \link qdawg-node.html Node\endlink of the DAWG.
*/
const QDawg::Node* QDawg::root() const
{
    return d ? d->root() : 0;
}

/*!
  Returns TRUE if the DAWG contains the word \a s; otherwise returns
  FALSE.
*/
bool QDawg::contains(const QString& s) const
{
    return d ? d->contains(s) : FALSE;
}

/*!
  \internal

  For debugging: prints out the DAWG contents.
*/
void QDawg::dump() const
{
    if ( d ) d->dump();
}

/*!
  \class QDawg::Node qdawg.h
  \brief The QDawg::Node class represents one node of a QDawg.
*/

/*!
  \fn QChar QDawg::Node::letter() const

  Returns this Node's letter.
*/
/*!
  \fn bool QDawg::Node::isWord() const

  Returns TRUE if this Node is the end of a word; otherwise returns
  FALSE.
*/
/*!
  \fn bool QDawg::Node::isLast() const

  Returns TRUE if this Node is the last in the child list; otherwise
  returns FALSE.
*/
/*!
  \fn const Node* QDawg::Node::next() const

  Returns the next child Node in the child list or 0 if the current
  Node isLast().
*/
/*!
  \fn const Node* QDawg::Node::jump() const

  Returns the node connected to this Node.
*/
