/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include "xmlreader.h"

Node::Node()
    : parent( 0 ), prev( 0 ),
      next( 0 ), first( 0 ), last( 0 )
{
}


Node::~Node()
{
    Node *n = first, *m;

    while ( n ) {
	m = n->next;
	delete n;
	n = m;
    }
}


void Node::addChild( Node *child )
{
    child->parent = this;

    if ( last )
	last->next = child;
    child->prev = last;

    if ( !first )
	first = child;
    last = child;
}

QString Node::attribute( const QString& name )
{
    return attributes[name];
}

void Node::setAttributes( const QXmlAttributes &a )
{
    for ( int i = 0; i < a.length(); i++ )
	attributes[ a.qName( i ) ] = a.value( i );
}

QMap<QString, QString> Node::attributeMap()
{
    return attributes;
}

QString Node::subData(const QString& tag) const
{
    Node* c = firstChild();
    while ( c ) {
	if ( c->tagName() == tag )
	    return c->data();
	c = c->nextNode();
    }
    return QString::null;
}

XmlHandler::XmlHandler()
    : node( 0 ), tree( 0 )
{
}

XmlHandler::~XmlHandler()
{
}


bool XmlHandler::startDocument()
{
    tree = node = new Node;
    node->setTagName( "DOCUMENT" );

    return TRUE;
}


bool XmlHandler::endDocument()
{
    if ( node != tree )
	return FALSE;

    return TRUE;
}

bool XmlHandler::startElement( const QString &, const QString &,
			       const QString &qName, const QXmlAttributes &attr )
{
    Node *nnode = new Node;
    nnode->setAttributes( attr );
    nnode->setTagName( qName );

    node->addChild( nnode );
    node = nnode;

    return TRUE;
}


bool XmlHandler::endElement( const QString &, const QString &, const QString & )
{
    if ( node == tree )
	return FALSE;

    node = node->parentNode();
    return TRUE;
}


bool XmlHandler::characters( const QString &ch )
{
    node->appendData( ch );

    return TRUE;
}
