/*
                             This file is part of the Opie Project
                             Copyright (C) Holger Freyther <zecke@handhelds.org>
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

/*
 This work is derived from:
 ----
 The Loki Library
 Copyright (c) 2001 by Andrei Alexandrescu
 This code accompanies the book:
 Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
 Permission to use, copy, modify, distribute and sell this software for any
     purpose is hereby granted without fee, provided that the above copyright
     notice appear in all copies and that both that copyright notice and this
     permission notice appear in supporting documentation.
 The author or Addison-Welsey Longman make no representations about the
     suitability of this software for any purpose. It is provided "as is"
     without express or implied warranty.
 ----

  And KGenericFactor et all from Simon Hausmann <tronical@kde.org>

*/

#include <qstring.h>
#include <qmetaobject.h>

#include <qtopia/qcom.h>
#include <qtopia/applicationinterface.h>

namespace Opie {
namespace Core {
    struct NullType;

    template <class T, class U>
    struct Typelist
    {
       typedef T Head;
       typedef U Tail;
    };
    template<
        typename T1  = NullType, typename T2  = NullType, typename T3  = NullType,
        typename T4  = NullType, typename T5  = NullType, typename T6  = NullType,
        typename T7  = NullType, typename T8  = NullType, typename T9  = NullType,
        typename T10 = NullType, typename T11 = NullType, typename T12 = NullType,
        typename T13 = NullType, typename T14 = NullType, typename T15 = NullType,
        typename T16 = NullType, typename T17 = NullType, typename T18 = NullType
        >
    struct MakeTypelist{
    private:
    typedef typename MakeTypelist
    <
        T2 , T3 , T4 ,
        T5 , T6 , T7 ,
        T8 , T9 , T10,
        T11, T12, T13,
        T14, T15, T16,
        T17, T18
        >
    ::Result TailResult;

public:
    typedef Typelist<T1, TailResult> Result;
};

template<>
struct MakeTypelist<>
{
    typedef NullType Result;
};


/**
 * To allow your application to be quick launched some one needs
 * to create the QWidget.
 * This is this factory. Make surce your widget has static QString Widget::appName()
 * as one of its functions.
 *
 * This template takes one QWidget and initialized it in the form of
 *  MyWidget::MyWidget( QWidget* parent, const char* name, WFlags f );
 *
 * To use it on your app do that:
 * typedef OApplicationFactory<MyWidget> MyFactory;
 * OPIE_EXPORT_APP( MyFactory )
 *
 */
template <class Product>
struct OApplicationFactory : public ApplicationInterface {
    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
        *iface = 0;
        if ( uuid == IID_QUnknown ) *iface = this;
        else if ( uuid == IID_QtopiaApplication ) *iface = this;
        else return QS_FALSE;
        (*iface)->addRef();
        return QS_OK;
    }

    /*
     *
     */
    virtual QWidget *createMainWindow( const QString& appName, QWidget* parent,
                                       const char* name, Qt::WFlags f ) {
        if (appName == Product::appName() )
            return new Product(parent, name, f );
        else
            return 0l;
    }

    virtual QStringList applications()const {
        QStringList list;
        list << Product::appName() ;

        return list;
    }
    Q_REFCOUNT

};


/* Internal */

template< class Product >
struct OPrivate {
    inline static QWidget *multiFactory( const QString& appName, QWidget* parent,
                           const char* name, Qt::WFlags fl ) {
        if ( appName == Product::appName() )
            return new Product( parent, name, fl );
        else
            return 0;
    }

    inline static QStringList multiString( const QStringList& _list ) {
        QStringList list = _list;
        list << Product::appName();
        return list;
    }
};

template <>
struct OPrivate<Opie::Core::NullType > {
    inline static QWidget* multiFactory ( const QString& , QWidget* ,
                            const char* , Qt::WFlags ) {
        return 0l;
    }
    inline static QStringList multiString( const QStringList& _list ) {
        return _list;
    }
};

/*
template <>
struct OPrivate <Opie::NullType, Opie::NullType > {
    inline static QWidget* multiFactory( const QString& , QWidget* ,
                           const char* , Qt::WFlags  ) {
        return 0l;
    }

    inline static QStringList multiString( const QStringList& _list ) {
        return _list;
    }
};
*/

template <class Product, class ProductListTail>
struct OPrivate< Opie::Core::Typelist<Product, ProductListTail> > {
    inline static QWidget* multiFactory( const QString& appName, QWidget* parent,
                           const char* name, Qt::WFlags fl) {
        QWidget* wid = OPrivate<Product>::multiFactory( appName, parent, name, fl );

        if (!wid )
            wid = OPrivate<ProductListTail>::multiFactory( appName, parent, name, fl );

        return wid;
    }

    inline static QStringList multiString( const QStringList& _list ) {
        QStringList list = _list;

        list = OPrivate<Product>::multiString( list );
        list = OPrivate<ProductListTail>::multiString( list );

        return list;
    }
};








/* Internal END */

/*
 * If you want to export more than one Widget use that function
 * Make sure all your Widgets provide the appName() static method
 * otherwise you'll get a compiler error
 *
 * typedef Opie::MakeTypeList<MyWidget, MyDialog, MyMediaPlayer >::Result MyTypes;
 * OPIE_EXPORT_APP( OApplicationFactory<MyTypes> )
 */

template<class Product, class ProductListTail>
struct OApplicationFactory< Opie::Core::Typelist<Product, ProductListTail > >
    : ApplicationInterface {
    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
        *iface = 0;
        if ( uuid == IID_QUnknown ) *iface = this;
        else if ( uuid ==IID_QtopiaApplication ) *iface = this;
        else return QS_FALSE;
        (*iface)->addRef();
        return QS_OK;
    }

    QWidget* createMainWindow ( const QString& appName, QWidget* parent,
                                const char* name, Qt::WFlags fl ) {
        qWarning("StringList is %s", applications().join(":").latin1() );
        return OPrivate< Opie::Core::Typelist<Product, ProductListTail > >::multiFactory( appName, parent, name, fl );
    }

    QStringList applications()const {
        QStringList _list;
        return OPrivate< Opie::Core::Typelist<Product, ProductListTail> >::multiString( _list );
    }

    Q_REFCOUNT
};

}
}

/* If the library version should be build */
#ifdef OPIE_APP_INTERFACE
#define OPIE_EXPORT_APP( factory ) Q_EXPORT_INTERFACE() { Q_CREATE_INSTANCE( factory ) }
#else

#include <qpe/qpeapplication.h>

#define OPIE_EXPORT_APP( Factory )                                      \
int main( int argc,  char **argv ) {                                    \
    QPEApplication a(argc, argv );               \
    QWidget *mw = 0;\
\
    /* method from TT */ \
    QString executableName = QString::fromLatin1( argv[0] ); \
    executableName =  executableName.right(executableName.length() \
	    - executableName.findRev('/') - 1); \
 \
    Factory f; \
    QStringList list = f.applications(); \
    if (list.contains(executableName) ) \
        mw = f.createMainWindow(executableName, 0, 0, 0 ); \
    else \
        mw = f.createMainWindow( list[0], 0, 0, 0 ); \
\
    if( mw ) { \
        if ( mw->metaObject()->slotNames().contains("setDocument(const QString&)" ) ) \
            a.showMainDocumentWidget( mw ); \
        else \
            a.showMainWidget( mw ); \
\
        int rv = a.exec(); \
        delete mw; \
        return rv; \
    }else \
        return -1; \
}
#endif

#ifdef OPIE_APP_INTERFACE
#define OPIE_EXPORT_APP_V2( factory,name ) Q_EXPORT_INTERFACE() { Q_CREATE_INSTANCE( factory ) }
#else

#include <opie2/oapplication.h>

#define OPIE_EXPORT_APP_V2( Factory,name )                                      \
int main( int argc,  char **argv ) {                                    \
    Opie::Core::OApplication a(argc, argv, name );               \
    QWidget *mw = 0;\
\
    /* method from TT */ \
    QString executableName = QString::fromLatin1( argv[0] ); \
    executableName =  executableName.right(executableName.length() \
	    - executableName.findRev('/') - 1); \
 \
    Factory f; \
    QStringList list = f.applications(); \
    if (list.contains(executableName) ) \
        mw = f.createMainWindow(executableName, 0, 0, 0 ); \
    else \
        mw = f.createMainWindow( list[0], 0, 0, 0 ); \
\
    if( mw ) { \
        if ( mw->metaObject()->slotNames().contains("setDocument(const QString&)" ) ) \
            a.showMainDocumentWidget( mw ); \
        else \
            a.showMainWidget( mw ); \
\
        int rv = a.exec(); \
        delete mw; \
        return rv; \
    }else \
        return -1; \
}
#endif

