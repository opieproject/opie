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
#include <qtopia/private/palmtoprecord.h>
#include <qtopia/stringutil.h>
#include <qstringlist.h>

/*! \class Qtopia::PalmtopRecord palmtoprecord.h
  \brief The Qtopia::PalmtopRecord class is the base class for all PIM records.

  Provides unique id and category support for all PIM records.

  \ingroup qtopiaemb
  \ingroup qtopiadesktop
*/


namespace Qtopia {



Record &Record::operator=( const Record &c )
{
    mUid = c.mUid;
    mCats = c.mCats;
    customMap = c.customMap;
    return *this;
}

void Record::setCategories( int single )
{
    if ( single == 0 )
	return;
    mCats.resize(1);
    mCats[0] = single;
}

// convenience methods provided for loading and saving to xml
QString Record::idsToString( const QArray<int> &catsUnsorted )
{
    QArray<int> cats = catsUnsorted;
    cats.sort();

    QString str;
    for ( uint i = 0; i < cats.size(); i++ )
	if ( i == 0 )
	    str = QString::number( cats[int(i)] );
	else
	    str += ";" + QString::number( cats[int(i)] );

    return str;
}

// convenience methods provided for loading and saving to xml
QArray<int> Record::idsFromString( const QString &str )
{
    QStringList catStrs = QStringList::split( ";", str );
    QArray<int> cats( catStrs.count() );
    uint i = 0;
    for ( QStringList::ConstIterator it = catStrs.begin();
	  it != catStrs.end(); ++it ) {
	cats[int(i)] = (*it).toInt();
	i++;
    }
    return cats;
}

/*!
  Returns the string stored for the custom field \a key.
  Returns a null string if the field does not exist.
 */
QString Record::customField( const QString &key) const
{
    if (customMap.contains(key))
	return customMap[key];

    return QString::null;
}

/*!
  Sets the string stored for the custom field \a key to \a value.
 */
void Record::setCustomField( const QString &key, const QString &value)
{
//     qWarning("setting custom " + key + " to " + value);
    if (customMap.contains(key))
	customMap.replace(key, value);
    else
	customMap.insert(key, value);

//     qWarning(QString("custom size %1").arg(customMap.count()));
}

/*!
  Removes the custom field \a key.
 */
void Record::removeCustomField(const QString &key)
{
    customMap.remove(key);
}

QString Record::customToXml() const
{
    //qWarning(QString("writing custom %1").arg(customMap.count()));
    QString buf(" ");
    for ( QMap<QString, QString>::ConstIterator cit = customMap.begin();
	    cit != customMap.end(); ++cit) {
// 	qWarning(".ITEM.");
	buf += cit.key();
	buf += "=\"";
	buf += escapeString(cit.data());
	buf += "\" ";
    }
    return buf;
}

void Record::dump( const QMap<int, QString> &map )
{
    QMap<int, QString>::ConstIterator it;
    for( it = map.begin(); it != map.end(); ++it )
	qDebug("%d : %s",  it.key(),  it.data().local8Bit().data() );
}

}

