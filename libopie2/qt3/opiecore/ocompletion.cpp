/*
                             This file is part of the Opie Project
                             Originally part of the KDE Project
                             Copyright (C) 1999,2000,2001 Carsten Pfeiffer <pfeiffer@kde.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/ocompletion.h>

class OCompTreeNode;

/**************************************************************************************************/
/* OCompTreeNodeList
/**************************************************************************************************/

class OCompTreeNodeList
{
public:
    OCompTreeNodeList() : first(0), last(0), m_count(0) {}
    OCompTreeNode *begin() const { return first; }
    OCompTreeNode *end() const { return last; }

    OCompTreeNode *at(uint index) const;
    void append(OCompTreeNode *item);
    void prepend(OCompTreeNode *item);
    void insert(OCompTreeNode *after, OCompTreeNode *item);
    OCompTreeNode *remove(OCompTreeNode *item);
    uint count() const { return m_count; }

private:
    OCompTreeNode *first, *last;
    uint m_count;
};

typedef OCompTreeNodeList OCompTreeChildren;
typedef OSortableValueList<QString> OCompletionMatchesList;

/**
 * A helper class for OCompletion. Implements a tree of QChar.
 *
 * The tree looks like this (containing the items "kde", "kde-ui",
 * "kde-core" and "pfeiffer". Every item is delimited with QChar( 0x0 )
 *
 *              some_root_node
 *                  /     \
 *                 k       p
 *                 |       |
 *                 d       f
 *                 |       |
 *                 e       e
 *                /|       |
 *             0x0 -       i
 *                / \      |
 *               u   c     f
 *               |   |     |
 *               i   o     f
 *               |   |     |
 *              0x0  r     e
 *                   |     |
 *                   e     r
 *                   |     |
 *                  0x0   0x0
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 * @internal
 */

/**************************************************************************************************/
/* OCompTreeNode
/**************************************************************************************************/

class OCompTreeNode : public QChar
{
public:
    OCompTreeNode():QChar(), myWeight(0) {}
    OCompTreeNode( const QChar& ch, uint weight = 0 ):QChar( ch ), myWeight( weight ) {}
    ~OCompTreeNode();

    // FIXME: Do we need this for Opie? [see also the static ZoneAllocater below]
    //void * operator new( size_t s ) {
    //  return alloc.allocate( s );
    //}
    //void operator delete( void * s ) {
    //  alloc.deallocate( s );
    //}

    // Returns a child of this node matching ch, if available.
    // Otherwise, returns 0L
    inline OCompTreeNode * find( const QChar& ch ) const {
      OCompTreeNode * cur = myChildren.begin();
      while (cur && (*cur != ch)) cur = cur->next;
      return cur;
    }

    OCompTreeNode * insert( const QChar&, bool sorted );
    void remove( const QString& );

    inline int childrenCount() const { return myChildren.count(); };

    // weighting
    inline void confirm() { myWeight++; };
    inline void confirm(uint w) { myWeight += w; };
    inline void decline() { myWeight--; };
    inline uint weight() const { return myWeight; };

    inline const OCompTreeChildren * children() const { return &myChildren; };
    inline const OCompTreeNode * childAt(int index) const { return myChildren.at(index); };
    inline const OCompTreeNode * firstChild() const { return myChildren.begin(); };
    inline const OCompTreeNode * lastChild()  const { return myChildren.end(); };

    /* We want to handle a list of OCompTreeNodes on our own, to not
       need to use QValueList<>.  And to make it even more fast we don't
       use an accessor, but just a public member.  */
    OCompTreeNode *next;

private:
    uint myWeight;
    OCompTreeNodeList	myChildren;
    //static OZoneAllocator alloc; // FIXME: Do we need this for Opie?
};

/**************************************************************************************************/
/* OCompletionMatchesWrapper
/**************************************************************************************************/

class OCompletionMatchesWrapper
{
public:
    OCompletionMatchesWrapper( bool sort = false )
        : sortedList( sort ? new OCompletionMatchesList : 0L ),
          dirty( false )
    {}
    ~OCompletionMatchesWrapper() {
        delete sortedList;
    }

    void setSorting( bool sort ) {
        if ( sort && !sortedList )
            sortedList = new OCompletionMatchesList;
        else if ( !sort ) {
            delete sortedList;
            sortedList = 0L;
        }
        stringList.clear();
        dirty = false;
    }

    bool sorting() const {
        return sortedList != 0L;
    }

    void append( int i, const QString& string ) {
        if ( sortedList )
            sortedList->insert( i, string );
        else
            stringList.append( string );
        dirty = true;
    }

    void clear() {
        if ( sortedList )
            sortedList->clear();
        stringList.clear();
        dirty = false;
    }

    uint count() const {
        if ( sortedList )
            return sortedList->count();
        return stringList.count();
    }

    bool isEmpty() const {
        return count() == 0;
    }

    QString first() const {
        return list().first();
    }

    QString last() const {
        return list().last();
    }

    QStringList list() const;

    mutable QStringList stringList;
    OCompletionMatchesList *sortedList;
    mutable bool dirty;
};

/**************************************************************************************************/
/* OCompletionPrivate
/**************************************************************************************************/

class OCompletionPrivate
{
public:
    // not a member to avoid #including kcompletion_private.h from kcompletion.h
    // list used for nextMatch() and previousMatch()
    OCompletionMatchesWrapper matches;
};

/**************************************************************************************************/
/* OCompletion
/**************************************************************************************************/

OCompletion::OCompletion()
{
    d = new OCompletionPrivate;

    myCompletionMode = OGlobalSettings::completionMode();
    myTreeRoot = new OCompTreeNode;
    myBeep = true;
    myIgnoreCase = false;
    myHasMultipleMatches = false;
    myRotationIndex = 0;
    setOrder( Insertion );
}


OCompletion::~OCompletion()
{
    delete d;
    delete myTreeRoot;
}


void OCompletion::setOrder( CompOrder order )
{
    myOrder = order;
    d->matches.setSorting( order == Weighted );
}


void OCompletion::setIgnoreCase( bool ignoreCase )
{
    myIgnoreCase = ignoreCase;
}


void OCompletion::setItems( const QStringList& items )
{
    clear();
    insertItems( items );
}


void OCompletion::insertItems( const QStringList& items )
{
    bool weighted = (myOrder == Weighted);
    QStringList::ConstIterator it;
    if ( weighted ) { // determine weight
        for ( it = items.begin(); it != items.end(); ++it ) addWeightedItem( *it );
    }
    else {
        for ( it = items.begin(); it != items.end(); ++it ) addItem( *it, 0 );
    }
}


QStringList OCompletion::items() const
{
    OCompletionMatchesWrapper list; // unsorted
    bool addWeight = (myOrder == Weighted);
    extractStringsFromNode( myTreeRoot, QString::null, &list, addWeight );

    return list.list();
}


void OCompletion::addItem( const QString& item )
{
    d->matches.clear();
    myRotationIndex = 0;
    myLastString = QString::null;

    addItem( item, 0 );
}


void OCompletion::addItem( const QString& item, uint weight )
{
    if ( item.isEmpty() ) return;

    OCompTreeNode *node = myTreeRoot;
    uint len = item.length();

    bool sorted = (myOrder == Sorted);
    bool weighted = ((myOrder == Weighted) && weight > 1);

    // knowing the weight of an item, we simply add this weight to all of its
    // nodes.

    for ( uint i = 0; i < len; i++ ) {
        node = node->insert( item.at(i), sorted );
        if ( weighted ) node->confirm( weight -1 ); // node->insert() sets weighting to 1
    }

    // add 0x0-item as delimiter with evtl. weight
    node = node->insert( 0x0, true );
    if ( weighted )
        node->confirm( weight -1 );
    //qDebug( "OCompletion: added: %s (%i)", item.latin1(), node->weight());
}


void OCompletion::addWeightedItem( const QString& item )
{
    if ( myOrder != Weighted ) {
        addItem( item, 0 );
        return;
    }

    uint len = item.length();
    uint weight = 0;

    // find out the weighting of this item (appended to the string as ":num")
    int index = item.findRev(':');
    if ( index > 0 ) {
        bool ok;
        weight = item.mid( index + 1 ).toUInt( &ok );
        if ( !ok )
            weight = 0;

        len = index; // only insert until the ':'
    }

    addItem( item.left( len ), weight );
    return;
}


void OCompletion::removeItem( const QString& item )
{
    d->matches.clear();
    myRotationIndex = 0;
    myLastString = QString::null;

    myTreeRoot->remove( item );
}


void OCompletion::clear()
{
    d->matches.clear();
    myRotationIndex = 0;
    myLastString = QString::null;

    delete myTreeRoot;
    myTreeRoot = new OCompTreeNode;
}


QString OCompletion::makeCompletion( const QString& string )
{
    if ( myCompletionMode == OGlobalSettings::CompletionNone )
        return QString::null;

    //qDebug( "OCompletion: completing: %s", string );

    d->matches.clear();
    myRotationIndex = 0;
    myHasMultipleMatches = false;
    myLastMatch = myCurrentMatch;

    // in Shell-completion-mode, emit all matches when we get the same
    // complete-string twice
    if ( myCompletionMode == OGlobalSettings::CompletionShell &&
        string == myLastString ) {
        // Don't use d->matches since calling postProcessMatches()
        // on d->matches here would interfere with call to
        // postProcessMatch() during rotation

        findAllCompletions( string, &d->matches, myHasMultipleMatches );
            QStringList l = d->matches.list();
        postProcessMatches( &l );
        emit matches( l );

        if ( l.isEmpty() )
            doBeep( NoMatch );

        return QString::null;
    }

    QString completion;
    // in case-insensitive popup mode, we search all completions at once
    if ( myCompletionMode == OGlobalSettings::CompletionPopup ||
         myCompletionMode == OGlobalSettings::CompletionPopupAuto ) {
        findAllCompletions( string, &d->matches, myHasMultipleMatches );
        if ( !d->matches.isEmpty() )
            completion = d->matches.first();
    }
    else
        completion = findCompletion( string );

    if ( myHasMultipleMatches )
        emit multipleMatches();

    myLastString = string;
    myCurrentMatch = completion;

    postProcessMatch( &completion );

    if ( !string.isEmpty() ) { // only emit match when string != ""
        //qDebug( "OCompletion: Match: %s", completion );
        emit match( completion );
    }

    if ( completion.isNull() )
        doBeep( NoMatch );

    return completion;
}

QStringList OCompletion::substringCompletion( const QString& string ) const
{
    // get all items in the tree, eventually in sorted order
    bool sorted = (myOrder == Weighted);
    OCompletionMatchesWrapper allItems( sorted );
    extractStringsFromNode( myTreeRoot, QString::null, &allItems, false );

    QStringList list = allItems.list();

    // subStringMatches is invoked manually, via a shortcut, so we should
    // beep here, if necessary.
    if ( list.isEmpty() ) {
        doBeep( NoMatch );
        return list;
    }

    if ( string.isEmpty() ) { // shortcut
        postProcessMatches( &list );
        return list;
    }

    QStringList matches;
    QStringList::ConstIterator it = list.begin();

    for( ; it != list.end(); ++it ) {
        QString item = *it;
        if ( item.find( string, 0, false ) != -1 ) { // always case insensitive
            postProcessMatch( &item );
            matches.append( item );
        }
    }

    if ( matches.isEmpty() )
        doBeep( NoMatch );

    return matches;
}


void OCompletion::setCompletionMode( OGlobalSettings::Completion mode )
{
    myCompletionMode = mode;
}


QStringList OCompletion::allMatches()
{
    // Don't use d->matches since calling postProcessMatches()
    // on d->matches here would interfere with call to
    // postProcessMatch() during rotation
    OCompletionMatchesWrapper matches( myOrder == Weighted );
    bool dummy;
    findAllCompletions( myLastString, &matches, dummy );
    QStringList l = matches.list();
    postProcessMatches( &l );
    return l;
}


OCompletionMatches OCompletion::allWeightedMatches()
{
    // Don't use d->matches since calling postProcessMatches()
    // on d->matches here would interfere with call to
    // postProcessMatch() during rotation
    OCompletionMatchesWrapper matches( myOrder == Weighted );
    bool dummy;
    findAllCompletions( myLastString, &matches, dummy );
    OCompletionMatches ret( matches );
    postProcessMatches( &ret );
    return ret;
}

QStringList OCompletion::allMatches( const QString &string )
{
    OCompletionMatchesWrapper matches( myOrder == Weighted );
    bool dummy;
    findAllCompletions( string, &matches, dummy );
    QStringList l = matches.list();
    postProcessMatches( &l );
    return l;
}

OCompletionMatches OCompletion::allWeightedMatches( const QString &string )
{
    OCompletionMatchesWrapper matches( myOrder == Weighted );
    bool dummy;
    findAllCompletions( string, &matches, dummy );
    OCompletionMatches ret( matches );
    postProcessMatches( &ret );
    return ret;
}

/////////////////////////////////////////////////////
///////////////// tree operations ///////////////////


QString OCompletion::nextMatch()
{
    QString completion;
    myLastMatch = myCurrentMatch;

    if ( d->matches.isEmpty() ) {
        findAllCompletions( myLastString, &d->matches, myHasMultipleMatches );
        completion = d->matches.first();
        myCurrentMatch = completion;
            myRotationIndex = 0;
        postProcessMatch( &completion );
        emit match( completion );
        return completion;
    }

    QStringList matches = d->matches.list();
    myLastMatch = matches[ myRotationIndex++ ];

    if ( myRotationIndex == matches.count() -1 )
        doBeep( Rotation ); // indicate last matching item -> rotating

    else if ( myRotationIndex == matches.count() )
        myRotationIndex = 0;

    completion = matches[ myRotationIndex ];
    myCurrentMatch = completion;
    postProcessMatch( &completion );
    emit match( completion );
    return completion;
}



QString OCompletion::previousMatch()
{
    QString completion;
    myLastMatch = myCurrentMatch;

    if ( d->matches.isEmpty() ) {
        findAllCompletions( myLastString, &d->matches, myHasMultipleMatches );
        completion = d->matches.last();
        myCurrentMatch = completion;
            myRotationIndex = 0;
        postProcessMatch( &completion );
        emit match( completion );
        return completion;
    }

    QStringList matches = d->matches.list();
    myLastMatch = matches[ myRotationIndex ];
    if ( myRotationIndex == 1 )
        doBeep( Rotation ); // indicate first item -> rotating

    else if ( myRotationIndex == 0 )
        myRotationIndex = matches.count();

    myRotationIndex--;

    completion = matches[ myRotationIndex ];
    myCurrentMatch = completion;
    postProcessMatch( &completion );
    emit match( completion );
    return completion;
}



// tries to complete "string" from the tree-root
QString OCompletion::findCompletion( const QString& string )
{
    QChar ch;
    QString completion;
    const OCompTreeNode *node = myTreeRoot;

    // start at the tree-root and try to find the search-string
    for( uint i = 0; i < string.length(); i++ ) {
        ch = string.at( i );
        node = node->find( ch );

        if ( node )
            completion += ch;
        else
            return QString::null; // no completion
    }

    // Now we have the last node of the to be completed string.
    // Follow it as long as it has exactly one child (= longest possible
    // completion)

    while ( node->childrenCount() == 1 ) {
        node = node->firstChild();
        if ( !node->isNull() )
            completion += *node;
    }
    // if multiple matches and auto-completion mode
    // -> find the first complete match
    if ( node && node->childrenCount() > 1 ) {
        myHasMultipleMatches = true;

	if ( myCompletionMode == OGlobalSettings::CompletionAuto ) {
            myRotationIndex = 1;
            if (myOrder != Weighted) {
                while ( (node = node->firstChild()) ) {
                    if ( !node->isNull() )
                    completion += *node;
                        else
                    break;
                    }
                }
                else {
                // don't just find the "first" match, but the one with the
                // highest priority

                const OCompTreeNode* temp_node = 0L;
                while(1) {
                    int count = node->childrenCount();
                    temp_node = node->firstChild();
                    uint weight = temp_node->weight();
                    const OCompTreeNode* hit = temp_node;
                    for( int i = 1; i < count; i++ ) {
                    temp_node = node->childAt(i);
                    if( temp_node->weight() > weight ) {
                        hit = temp_node;
                        weight = hit->weight();
                    }
                    }
                    // 0x0 has the highest priority -> we have the best match
                    if ( hit->isNull() )
                    break;

                    node = hit;
                    completion += *node;
                    }
                }
            }

        else
            doBeep( PartialMatch ); // partial match -> beep
    }

    return completion;
}


void OCompletion::findAllCompletions(const QString& string,
                                     OCompletionMatchesWrapper *matches,
                                     bool& hasMultipleMatches) const
{
    //qDebug( "OCompletion: finding all completions for %s", (const char*) string );

    if ( string.isEmpty() )
        return;

    if ( myIgnoreCase ) { // case insensitive completion
        extractStringsFromNodeCI( myTreeRoot, QString::null, string, matches );
        hasMultipleMatches = (matches->count() > 1);
        return;
    }

    QChar ch;
    QString completion;
    const OCompTreeNode *node = myTreeRoot;

    // start at the tree-root and try to find the search-string
    for( uint i = 0; i < string.length(); i++ ) {
        ch = string.at( i );
        node = node->find( ch );

        if ( node )
            completion += ch;
        else
            return; // no completion -> return empty list
    }

    // Now we have the last node of the to be completed string.
    // Follow it as long as it has exactly one child (= longest possible
    // completion)

    while ( node->childrenCount() == 1 ) {
        node = node->firstChild();
        if ( !node->isNull() )
            completion += *node;
        // kdDebug() << completion << node->latin1();
    }


    // there is just one single match)
    if ( node->childrenCount() == 0 )
        matches->append( node->weight(), completion );

    else {
        // node has more than one child
        // -> recursively find all remaining completions
        hasMultipleMatches = true;
        extractStringsFromNode( node, completion, matches );
    }
}


void OCompletion::extractStringsFromNode( const OCompTreeNode *node,
                        const QString& beginning,
                        OCompletionMatchesWrapper *matches,
                        bool addWeight ) const
{
    if ( !node || !matches ) return;

    // kDebug() << "Beginning: " << beginning << endl;
    const OCompTreeChildren *list = node->children();
    QString string;
    QString w;

    // loop thru all children
    for ( OCompTreeNode *cur = list->begin(); cur ; cur = cur->next) {
        string = beginning;
        node = cur;
        if ( !node->isNull() )
            string += *node;

	while ( node && node->childrenCount() == 1 ) {
            node = node->firstChild();
            if ( node->isNull() ) break;
            string += *node;
        }

        if ( node && node->isNull() ) { // we found a leaf
            if ( addWeight ) {
                // add ":num" to the string to store the weighting
                string += ':';
                w.setNum( node->weight() );
                string.append( w );
                }
            matches->append( node->weight(), string );
        }

        // recursively find all other strings.
        if ( node && node->childrenCount() > 1 )
            extractStringsFromNode( node, string, matches, addWeight );
    }
}

void OCompletion::extractStringsFromNodeCI( const OCompTreeNode *node,
                                            const QString& beginning,
                                            const QString& restString,
                                      OCompletionMatchesWrapper *matches ) const
{
    if ( restString.isEmpty() ) {
        extractStringsFromNode( node, beginning, matches, false /*noweight*/ );
        return;
    }

    QChar ch1 = restString.at(0);
    QString newRest = restString.mid(1);
    OCompTreeNode *child1, *child2;

    child1 = node->find( ch1 ); // the correct match
    if ( child1 )
        extractStringsFromNodeCI( child1, beginning + *child1, newRest,
                                  matches );

    // append the case insensitive matches, if available
    if ( ch1.isLetter() ) {
        // find out if we have to lower or upper it. Is there a better way?
        QChar ch2 = ch1.lower();
        if ( ch1 == ch2 )
            ch2 = ch1.upper();
        if ( ch1 != ch2 ) {
            child2 = node->find( ch2 );
            if ( child2 )
                extractStringsFromNodeCI( child2, beginning + *child2, newRest,
                                          matches );
        }
    }
}

// FIXME: Revise this for Opie?

void OCompletion::doBeep( BeepMode mode ) const
{
    if ( !myBeep ) return;

    QString text, event;

    switch ( mode ) {
    case Rotation:
        event = QString::fromLatin1("Textcompletion: rotation");
        text = tr("You reached the end of the list\nof matching items.\n");
        break;
    case PartialMatch:
        if ( myCompletionMode == OGlobalSettings::CompletionShell ||
            myCompletionMode == OGlobalSettings::CompletionMan ) {
            event = QString::fromLatin1("Textcompletion: partial match");
            text = tr("The completion is ambiguous, more than one\nmatch is available.\n");
        }
        break;
    case NoMatch:
        if ( myCompletionMode == OGlobalSettings::CompletionShell ) {
            event = QString::fromLatin1("Textcompletion: no match");
            text = tr("There is no matching item available.\n");
        }
        break;
    }

    //if ( !text.isEmpty() )
        //ONotifyClient::event( event, text ); // FIXME: Revise for Opie?
}

// Implements the tree. Every node is a QChar and has a list of children, which
// are Nodes as well.
// QChar( 0x0 ) is used as the delimiter of a string; the last child of each
// inserted string is 0x0.

OCompTreeNode::~OCompTreeNode()
{
    // delete all children
    OCompTreeNode *cur = myChildren.begin();
    while (cur) {
        OCompTreeNode * next = cur->next;
        delete myChildren.remove(cur);
        cur = next;
    }
}


// Adds a child-node "ch" to this node. If such a node is already existant,
// it will not be created. Returns the new/existing node.
OCompTreeNode * OCompTreeNode::insert( const QChar& ch, bool sorted )
{
    OCompTreeNode *child = find( ch );
    if ( !child ) {
        child = new OCompTreeNode( ch );

	// FIXME, first (slow) sorted insertion implementation
	if ( sorted ) {
	    OCompTreeNode * prev = 0;
	    OCompTreeNode * cur = myChildren.begin();
	    while ( cur ) {
	        if ( ch > *cur ) {
		    prev = cur;
		    cur = cur->next;
		} else
		    break;
	    }
	    if (prev)
	        myChildren.insert( prev, child );
	    else
	        myChildren.prepend(child);
	}

	else
	    myChildren.append( child );
    }

    // implicit weighting: the more often an item is inserted, the higher
    // priority it gets.
    child->confirm();

    return child;
}


// Recursively removes a string from the tree (untested :-)
void OCompTreeNode::remove( const QString& string )
{
    OCompTreeNode *child = 0L;

    if ( string.isEmpty() ) {
        child = find( 0x0 );
        delete myChildren.remove( child );
        return;
    }

    QChar ch = string.at(0);
    child = find( ch );
    if ( child ) {
        child->remove( string.right( string.length() -1 ) );
        if ( child->myChildren.count() == 0 ) {
            delete myChildren.remove( child );
        }
    }
}

QStringList OCompletionMatchesWrapper::list() const {
    if ( sortedList && dirty ) {
        sortedList->sort();
        dirty = false;

        stringList.clear();

        // high weight == sorted last -> reverse the sorting here
        QValueListConstIterator<OSortableItem<QString> > it;
        for ( it = sortedList->begin(); it != sortedList->end(); ++it )
            stringList.prepend( (*it).value() );
    }

    return stringList;
}

OCompletionMatches::OCompletionMatches( bool sort_P )
    : _sorting( sort_P )
{
}

OCompletionMatches::OCompletionMatches( const OCompletionMatchesWrapper& matches )
    : _sorting( matches.sorting())
{
    if( matches.sortedList != 0L )
        OCompletionMatchesList::operator=( *matches.sortedList );
    else {
        QStringList l = matches.list();
        for( QStringList::ConstIterator it = l.begin();
             it != l.end();
             ++it )
            prepend( OSortableItem<QString, int>( 1, *it ) );
    }
}

OCompletionMatches::~OCompletionMatches()
{
}

QStringList OCompletionMatches::list( bool sort_P ) const
{
    if( _sorting && sort_P )
        const_cast< OCompletionMatches* >( this )->sort();
    QStringList stringList;
    // high weight == sorted last -> reverse the sorting here
    for ( ConstIterator it = begin(); it != end(); ++it )
        stringList.prepend( (*it).value() );
    return stringList;
}

void OCompletionMatches::removeDuplicates()
{
    Iterator it1, it2;
    for ( it1 = begin(); it1 != end(); ++it1 ) {
        for ( (it2 = it1), ++it2; it2 != end();) {
            if( (*it1).value() == (*it2).value()) {
                // use the max height
                //(*it1).first = kMax( (*it1).index(), (*it2).index());
                (*it1).first = (*it2).index() < (*it1).index() ? (*it1).index() : (*it2).index();
                it2 = remove( it2 );
                continue;
            }
            ++it2;
        }
    }
}

void OCompTreeNodeList::append(OCompTreeNode *item)
{
    m_count++;
    if (!last) {
        last = item;
        last->next = 0;
        first = item;
        return;
    }
    last->next = item;
    item->next = 0;
    last = item;
}

void OCompTreeNodeList::prepend(OCompTreeNode *item)
{
    m_count++;
    if (!last) {
        last = item;
        last->next = 0;
        first = item;
        return;
    }
    item->next = first;
    first = item;
}

void OCompTreeNodeList::insert(OCompTreeNode *after, OCompTreeNode *item)
{
    if (!after) {
        append(item);
        return;
    }

    m_count++;

    item->next = after->next;
    after->next = item;

    if (after == last)
	last = item;
}

OCompTreeNode *OCompTreeNodeList::remove(OCompTreeNode *item)
{
    if (!first || !item)
        return 0;
    OCompTreeNode *cur = 0;

    if (item == first)
        first = first->next;
    else {
        cur = first;
        while (cur && cur->next != item) cur = cur->next;
        if (!cur)
            return 0;
        cur->next = item->next;
    }
    if (item == last)
        last = cur;
    m_count--;
    return item;
}

OCompTreeNode *OCompTreeNodeList::at(uint index) const
{
    OCompTreeNode *cur = first;
    while (index-- && cur) cur = cur->next;
    return cur;
}

// FIXME: Revise for Opie?
//OZoneAllocator OCompTreeNode::alloc(8192);

//void OCompletion::virtual_hook( int, void* )
//{ /*BASE::virtual_hook( id, data );*/ }

//void OCompletionBase::virtual_hook( int, void* )
//{ /*BASE::virtual_hook( id, data );*/ }
