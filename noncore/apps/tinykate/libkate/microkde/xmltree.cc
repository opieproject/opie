/*  This file is part of the KDE project
    Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpe/stringutil.h>
#include <opie/xmltree.h>

#include <qxml.h>

#include <assert.h>

using namespace Opie;

XMLElement::XMLElement()
    : m_parent( 0 ), m_next( 0 ), m_prev( 0 ), m_first( 0 ), m_last( 0 )
{
}

XMLElement::~XMLElement()
{
    XMLElement *n = m_first;

    while ( n )
    {
        XMLElement *tmp = n;
        n = n->m_next;
        delete tmp;
    }
}

void XMLElement::appendChild( XMLElement *child )
{
    if ( child->m_parent )
        child->m_parent->removeChild( child );

    child->m_parent = this;

    if ( m_last )
        m_last->m_next = child;

    child->m_prev = m_last;

    if ( !m_first )
        m_first = child;

    m_last = child;
}

void XMLElement::insertAfter( XMLElement *newChild, XMLElement *refChild )
{
    assert( newChild != refChild );

    if ( refChild == m_last )
    {
        appendChild( newChild );
        return;
    }

    assert( refChild );
    assert( refChild->m_parent );
    assert( refChild->m_parent == this );

    if ( newChild->m_parent && newChild != refChild )
        newChild->m_parent->removeChild( newChild );

    newChild->m_parent = this;

    XMLElement *next = refChild->m_next;

    refChild->m_next = newChild;

    newChild->m_prev = refChild;
    newChild->m_next = next;
   
    if ( next )
        next->m_prev = newChild;
}

QString XMLElement::attribute( const QString &attr ) const
{
    AttributeMap::ConstIterator it = m_attributes.find( attr );
    if ( it == m_attributes.end() )
	return QString::null;
    return it.data();
}

void XMLElement::setAttribute( const QString &attr, const QString &value )
{
    m_attributes.replace( attr, value );
}

void XMLElement::insertBefore( XMLElement *newChild, XMLElement *refChild )
{
    assert( refChild );
    assert( refChild->m_parent );
    assert( refChild->m_parent == this );
    assert( newChild != refChild );

    if ( newChild->m_parent && newChild != refChild )
        newChild->m_parent->removeChild( newChild );

    newChild->m_parent = this;

    XMLElement *prev = refChild->m_prev;

    refChild->m_prev = newChild;
    
    newChild->m_prev = prev;
    newChild->m_next = refChild;

    if ( prev )
        prev->m_next = newChild;

    if ( refChild == m_first )
        m_first = newChild;
}

void XMLElement::removeChild( XMLElement *child )
{
    if ( child->m_parent != this )
        return;

    if ( m_first == child )
        m_first = child->m_next;

    if ( m_last == child )
        m_last = child->m_prev;

    if ( child->m_prev )
        child->m_prev->m_next = child->m_next;

    if ( child->m_next )
        child->m_next->m_prev = child->m_prev;

    child->m_parent = 0;
    child->m_prev = 0;
    child->m_next = 0;
}

void XMLElement::save( QTextStream &s, uint indent )
{
    if ( !m_value.isEmpty() )
    {
        s << Qtopia::escapeString( m_value );
        return;
    }

    for ( uint i = 0; i < indent; ++i )
        s << " ";

    s << "<" << m_tag;

    if ( !m_attributes.isEmpty() )
    {
        s << " ";
        AttributeMap::ConstIterator it = m_attributes.begin();
        AttributeMap::ConstIterator end = m_attributes.end();
        for (; it != end; ++it )
        {
            s << it.key() << "=\"" << Qtopia::escapeString( it.data() ) << "\"";
            s << " ";
        }
    }

    if ( m_last )
    {
        if ( ( m_first && !m_first->value().isEmpty() ) || !m_parent )
            s << ">";
        else
            s << ">" << endl;

        int newIndent = indent;
        if ( m_parent )
            newIndent++;

        XMLElement *n = m_first;
        while ( n )
        {
            n->save( s, newIndent );
            n = n->nextChild();
        }

        if ( m_last && m_last->value().isEmpty() && m_parent )
            for ( uint i = 0; i < indent; ++i )
                s << " ";

        if ( m_parent )
            s << "</" << m_tag << ">" << endl;
    }
    else
        s << "/>" << endl;
}

class Handler : public QXmlDefaultHandler
{
public:
    Handler() : m_node( 0 ), m_root( 0 ) {}

    XMLElement *root() const { return m_root; }

    virtual bool startDocument();
    virtual bool endDocument();
    virtual bool startElement( const QString &ns, const QString &ln, const QString &qName,
                               const QXmlAttributes &attr );
    virtual bool endElement( const QString &ns, const QString &ln, const QString &qName );
    virtual bool characters( const QString &ch );

private:
    XMLElement *m_node;
    XMLElement *m_root;
};

bool Handler::startDocument()
{
    m_root = m_node = new XMLElement;

    return true;
}

bool Handler::endDocument()
{
    return m_root == m_node;
}

bool Handler::startElement( const QString &, const QString &, const QString &qName,
                           const QXmlAttributes &attr )
{
    XMLElement *bm = new XMLElement;

    XMLElement::AttributeMap attributes;
    for ( int i = 0; i < attr.length(); ++i )
        attributes[ attr.qName( i ) ] = attr.value( i );

    bm->setAttributes( attributes );

    bm->setTagName( qName );

    m_node->appendChild( bm );
    m_node = bm;

    return true;
}

bool Handler::endElement( const QString &, const QString &, const QString & )
{
    if ( m_node == m_root )
        return false;

    m_node = m_node->parent();
    return true;
}

bool Handler::characters( const QString &ch )
{
    XMLElement *textNode = new XMLElement;
    textNode->setValue( ch );
    m_node->appendChild( textNode );
    return true;
}

XMLElement *XMLElement::namedItem( const QString &name )
{
    XMLElement *e = m_first;

    for (; e; e = e->nextChild() )
        if ( e->tagName() == name )
            return e;

    return 0; 
}

XMLElement *XMLElement::clone() const
{
    XMLElement *res = new XMLElement;

    res->setTagName( m_tag );
    res->setValue( m_value );
    res->setAttributes( m_attributes );

    XMLElement *e = m_first;
    for (; e; e = e->m_next )
        res->appendChild( e->clone() );

    return res;
}

XMLElement *XMLElement::load( const QString &fileName )
{
    QFile f( fileName );
    if ( !f.open( IO_ReadOnly ) )
        return 0;

    QTextStream stream( &f );
    stream.setEncoding( QTextStream::UnicodeUTF8 );
    QXmlInputSource src( stream );
    QXmlSimpleReader reader;
    Handler handler;

    reader.setFeature( "http://trolltech.com/xml/features/report-whitespace-only-CharData", false );
    reader.setContentHandler( &handler );
    reader.parse( src );

    return handler.root();;
}

/* vim: et sw=4
 */
