#include "QSafeDataStream.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>

QSafeDataStream &QSafeDataStream::operator>>( Q_INT8 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( Q_UINT8 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( Q_INT16 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( Q_UINT16 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( Q_INT32 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( Q_UINT32 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( Q_INT64 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( Q_UINT64 &i )
{
	if(atEnd()){
		i = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(i);
	}
}


QSafeDataStream &QSafeDataStream::operator>>( float &f )
{
	if(atEnd()){
		f = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(f);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( double &f )
{
	if(atEnd()){
		f = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(f);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( char *&str )
{
	if(atEnd()){
		str = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::operator>>(str);
	}
}

QSafeDataStream &QSafeDataStream::readBytes( char *&s, uint &len )
{
	if(atEnd()){
		s = 0;
		len = 0;
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::readBytes(s, len);
	}
}

QSafeDataStream &QSafeDataStream::readRawBytes( char *s, uint len )
{
	if(atEnd()){
		return *this;
	} else {
		return (QSafeDataStream&)QDataStream::readRawBytes(s, len);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( QString& s )
{
	if(atEnd()){
		s = QString::null;
		return *this;
	} else {
		return (QSafeDataStream&)(*((QDataStream*)this) >> s);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( QStringList& list )
{
	if(atEnd()){
		list.clear();
		return *this;
	} else {
		return (QSafeDataStream&)(*((QDataStream*)this) >> list);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( QByteArray& a )
{
	if(atEnd()){
		a.resize(0);
		return *this;
	} else {
		return (QSafeDataStream&)(*((QDataStream*)this) >> a);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( QCString& s )
{
	if(atEnd()){
		s.resize(0);
		return *this;
	} else {
		return (QSafeDataStream&)(*((QDataStream*)this) >> s);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( QDate& d )
{
	if(atEnd()){
		return *this;
	} else {
		return (QSafeDataStream&)(*((QDataStream*)this) >> d);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( QTime& t )
{
	if(atEnd()){
		return *this;
	} else {
		return (QSafeDataStream&)(*((QDataStream*)this) >> t);
	}
}

QSafeDataStream &QSafeDataStream::operator>>( QDateTime& dt )
{
	if(atEnd()){
		return *this;
	} else {
		return (QSafeDataStream&)(*((QDataStream*)this) >> dt);
	}
}
