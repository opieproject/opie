/*
                             This file is part of the Opie Project

                             Copyright (C) 2005 Holger Hans Peter Freyther <freyther@handhelds.org>
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

#ifndef ANET_NODE_INTERFACE_H
#define ANET_NODE_INTERFACE_H

#include <opie2/oapplicationfactory.h>
#include <qpe/qcom.h>

// {A215A785-FB73-4F74-84B0-053BCC77DB87}
#ifndef IID_NetworkSettings2

#define IID_NetworkSettings2 QUuid( 0xa215a785, 0xfb73, 0x4f74, 0x84, 0xb0, 0x05, 0x3b, 0xcc, 0x77, 0xdb, 0x87)

#endif



/**
 * Multiple Types
 */
template <class Node >
struct NS2PrivateFactory {

    inline static void createPlugins( QList<ANetNode> & PNN) {
	PNN.append( new Node());
    }

};

/*
 * Stop recursion here
 */
template <>
struct NS2PrivateFactory<Opie::Core::NullType> {

    inline static void createPlugins( QList<ANetNode> &) {

    }

};

template <class Node, class Tail>
struct NS2PrivateFactory<Opie::Core::Typelist<Node, Tail> > {

    inline static void createPlugins( QList<ANetNode> & PNN ) {
	NS2PrivateFactory<Node>::createPlugins(PNN);
	NS2PrivateFactory<Tail>::createPlugins(PNN);
    }

};

class NetNodeInterface : public QUnknownInterface {

public :

      virtual QRESULT queryInterface( const QUuid& uuid, 
                              QUnknownInterface **iface ) = 0;

      virtual void create_plugin( QList<ANetNode> & PNN ) = 0;
};

template<class Node>
struct NetNodeInterface_T : public NetNodeInterface {

    QRESULT queryInterface(const QUuid& uuid, QUnknownInterface **iface) {
	*iface = 0;

	if( uuid == IID_QUnknown ) 
	  *iface = this;
	else if( uuid == IID_NetworkSettings2 ) 
	  *iface = this;
	else 
	  return QS_FALSE;

	(*iface)->addRef();

	return QS_OK;

    }

    void create_plugin( QList<ANetNode> & PNN ) {

	PNN.append( new Node());

    }

    Q_REFCOUNT
};

template<class Node, class Tail>
struct NetNodeInterface_T<Opie::Core::Typelist<Node, Tail> >
    : public NetNodeInterface {

    QRESULT queryInterface( const QUuid& uuid, 
                            QUnknownInterface **iface) {

	*iface = 0;

	if( uuid == IID_QUnknown ) *iface = this;
	else if( uuid == IID_NetworkSettings2 ) *iface = this;
	else return QS_FALSE;

	(*iface)->addRef();
	return QS_OK;

    }

    void create_plugin( QList<ANetNode> & PNN ) {
	NS2PrivateFactory<Opie::Core::Typelist<Node,Tail> >::createPlugins( PNN );
    }

    Q_REFCOUNT

};

#define OPIE_NS2_PLUGIN( factory ) \
          Q_EXPORT_INTERFACE() { Q_CREATE_INSTANCE( factory) }

#endif

