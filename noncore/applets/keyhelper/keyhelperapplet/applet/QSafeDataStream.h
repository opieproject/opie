#ifndef _QSAFEDATASTREAM_H_
#define _QSAFEDATASTREAM_H_

#include <qdatastream.h>

class QString;
class QStringList;
class QCString;
class QDate;
class QTime;
class QDateTime;

class QSafeDataStream : public QDataStream
{
public:
	/* constructors */
	QSafeDataStream()
		: QDataStream() {}
	QSafeDataStream(QIODevice* d)
		: QDataStream(d) {}
	QSafeDataStream(QByteArray a, int mode)
		: QDataStream(a, mode) {}

    /* read functions */
    QSafeDataStream &operator>>( Q_INT8 &i );
    QSafeDataStream &operator>>( Q_UINT8 &i );
    QSafeDataStream &operator>>( Q_INT16 &i );
    QSafeDataStream &operator>>( Q_UINT16 &i );
    QSafeDataStream &operator>>( Q_INT32 &i );
    QSafeDataStream &operator>>( Q_UINT32 &i );
    QSafeDataStream &operator>>( Q_INT64 &i );
    QSafeDataStream &operator>>( Q_UINT64 &i );

    QSafeDataStream &operator>>( float &f );
    QSafeDataStream &operator>>( double &f );
    QSafeDataStream &operator>>( char *&str );

    QSafeDataStream &readBytes( char *&, uint &len );
    QSafeDataStream &readRawBytes( char *, uint len );

    QSafeDataStream &operator>>( QString& s );
	QSafeDataStream &operator>>( QStringList& list );
	QSafeDataStream &operator>>( QByteArray& a );
	QSafeDataStream &operator>>( QCString& s );
	QSafeDataStream &operator>>( QDate& d );
	QSafeDataStream &operator>>( QTime& t );
	QSafeDataStream &operator>>( QDateTime& dt );

};

#endif /* _QSAFEDATASTREAM_H_ */
