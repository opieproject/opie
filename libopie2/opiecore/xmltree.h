/*  This file is part of the KDE project
    Copyright (C) 2000,2001 Simon Hausmann <hausmann@kde.org>

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


#ifndef __bookmarks_h__
#define __bookmarks_h__

#include <qstring.h>
#include <qmap.h>
#include <qtextstream.h>

namespace Opie
{

/**
 *  A small xml lib written by Simon Hausmann.
 */ 
class XMLElement
{
public:
    typedef QMap<QString, QString> AttributeMap;

    /**
     *  The constructor of XMLElement
     */
    XMLElement();
    ~XMLElement();

    /** appendChild appends a child to the XMLElement behind the last element.
     *  The ownership of the child get's transfered to the
     *  this XMLElement.
     *  If child is already the child of another parent
     *  it's get removed from the other parent first. 
     */
    void appendChild( XMLElement *child );

    /** inserts newChild after refChild. If newChild is the child
     *  of another parent the child will get removed.
     *  The ownership of child gets transfered.
     *
     */
    void insertAfter( XMLElement *newChild, XMLElement *refChild );

    /** same as insertAfter but the element get's inserted before refChild.
     *
     */
    void insertBefore( XMLElement *newChild, XMLElement *refChild );

    /** removeChild removes the child from the XMLElement.
     *  The ownership gets dropped. You need to delete the
     *  child yourself.
     */
    void removeChild( XMLElement *child );

    /** parent() returns the parent of this XMLElement
     *  If there is no parent 0l gets returned
     */
    XMLElement *parent() const { return m_parent; }
    XMLElement *firstChild() const { return m_first; }
    XMLElement *nextChild() const { return m_next; }
    XMLElement *prevChild() const { return m_prev; }
    XMLElement *lastChild() const { return m_last; }

    void setTagName( const QString &tag ) { m_tag = tag; }
    QString tagName() const { return m_tag; }

    void setValue( const QString &val ) { m_value = val; }
    QString value() const { return m_value; }

    void setAttributes( const AttributeMap &attrs ) { m_attributes = attrs; }
    AttributeMap attributes() const { return m_attributes; }
    AttributeMap &attributes() { return m_attributes; }

    QString attribute( const QString & ) const;
    void setAttribute( const QString &attr, const QString &value );
    void save( QTextStream &stream, uint indent = 0 );

    XMLElement *namedItem( const QString &name );

    XMLElement *clone() const;

    static XMLElement *load( const QString &fileName );

private:
    QString m_tag;
    QString m_value;
    AttributeMap m_attributes;

    XMLElement *m_parent;
    XMLElement *m_next;
    XMLElement *m_prev;
    XMLElement *m_first;
    XMLElement *m_last;

    XMLElement( const XMLElement &rhs );
    XMLElement &operator=( const XMLElement &rhs );
};

} // namespace Opie

#endif
