#include  "jpeg_slave.h"
#include "thumbnailtool.h"

PHUNK_VIEW_INTERFACE( "JPEG", JpegSlave )

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/opieexif.h>
#include <qpe/timestring.h>
using namespace Opie::Core;
using namespace Opie::MM;

/* QT */
#include <qobject.h>
#include <qimage.h>
static QDateTime parseDateTime( const QString& string )
{
    QDateTime dt;
    if ( string.length() != 19 )
        return dt;

    QString year    = string.left( 4 );
    QString month   = string.mid( 5, 2 );
    QString day     = string.mid( 8, 2 );
    QString hour    = string.mid( 11, 2 );
    QString minute  = string.mid( 14, 2 );
    QString seconds = string.mid( 18, 2 );

    bool ok;
    bool allOk = true;
    int y  = year.toInt( &ok );
    allOk &= ok;

    int mo = month.toInt( &ok );
    allOk &= ok;

    int d  = day.toInt( &ok );
    allOk &= ok;

    int h  = hour.toInt( &ok );
    allOk &= ok;

    int mi = minute.toInt( &ok );
    allOk &= ok;

    int s  = seconds.toInt( &ok );
    allOk &= ok;

    if ( allOk ) {
        dt.setDate( QDate( y, mo, d ) );
        dt.setTime( QTime( h, mi, s ) );
    }

    return dt;
}

JpegSlave::JpegSlave()
    : SlaveInterface(  QStringList::split( " ", "jpeg jpg" ) )
{}

JpegSlave::~JpegSlave() {}

QString JpegSlave::iconViewName( const QString& path) {
    ExifData ImageInfo;
    if ( !ImageInfo.scan( path ) )
        return QString::null;

    QString tag;
    tag  = QObject::tr( "Comment: %1\n" ).arg( ImageInfo.getComment() );
    {
// ODP fixme
        QString timestring = TimeString::dateString( parseDateTime(  ImageInfo.getDateTime() ), FALSE );
        tag += QObject::tr( "Date/Time: %1\n" ).arg( timestring );
    }
    tag += QObject::tr( "Dimensions: %1x%2\n" ).arg(ImageInfo.getWidth())
           .arg(ImageInfo.getHeight() );

    tag += ExifData::color_mode_to_string(  ImageInfo.getIsColor() );

    tag += ExifData::compression_to_string( ImageInfo.getCompressionLevel() );

    return tag;
}


/*
 * messy messy string creation
 */
QString JpegSlave::fullImageInfo( const QString& path) {
    ExifData ImageInfo;
    if ( !ImageInfo.scan( path ) )
        return QString::null;

    QString tag, tmp;
    tag   = QObject::tr( "<qt>Comment: %1\n" ).arg( ImageInfo.getComment() );

    tmp =  ImageInfo.getCameraMake();
    if ( tmp.length() )
        tag  += QObject::tr( "Manufacturer: %1\n" ).arg( tmp );
    tmp = ImageInfo.getCameraModel();
    if ( tmp.length() )
        tag += QObject::tr( "Model: %1\n" ).arg( tmp );
    {
// ODP fixme
        tmp = TimeString::dateString( parseDateTime(  ImageInfo.getDateTime() ), FALSE );
        tag += QObject::tr( "Date/Time: %1\n" ).arg( tmp );
    }
    tag += QObject::tr( "Dimensions: %1x%2\n" ).arg(ImageInfo.getWidth())
           .arg(ImageInfo.getHeight() );

    tag += ExifData::color_mode_to_string(  ImageInfo.getIsColor() );

    tag += ExifData::compression_to_string( ImageInfo.getCompressionLevel() );
    if ( ImageInfo.getOrientation() )
        tag += QObject::tr( "Orientation: %1\n" ).arg(ImageInfo.getOrientation() );


    {
        int flash_used = ImageInfo.getFlashUsed();
        if ( flash_used >= 0 )
            tag += QObject::tr( "Flash used\n" );
    }

    if ( ImageInfo.getFocalLength() ) {
        tag += QObject::tr( "Focal length: %1\n" ).arg(  QString().sprintf( "%4.1f", ImageInfo.getFocalLength() ) );
        if ( ImageInfo.getCCDWidth() )
            tag += QObject::tr( "35mm equivalent: %1\n" ).arg(  (int)(ImageInfo.getFocalLength()/ImageInfo.getCCDWidth()*35 + 0.5) );

    }

    if ( ImageInfo.getCCDWidth() )
        tag += QObject::tr( "CCD width: %1" ).arg( ImageInfo.getCCDWidth() );
    if ( ImageInfo.getExposureTime() ) {
        tmp = QString().sprintf("%4.2f", ImageInfo.getExposureTime() );
        float exposureTime = ImageInfo.getExposureTime();
        if ( exposureTime > 0 && exposureTime <= 0.5 )
            tmp += QString().sprintf(" (1/%d)", (int)(0.5 +1/exposureTime) );
        tag += QObject::tr( "Exposure time: %1\n" ).arg( tmp );
    }

    if ( ImageInfo.getApertureFNumber() )
        tag += QObject::tr( "Aperture: %1\n" ).arg(  QString().sprintf("f/%3.1f", (double)ImageInfo.getApertureFNumber() ) );

    if ( ImageInfo.getDistance() ) {
        if ( ImageInfo.getDistance() < 0 )
            tag += QObject::tr( "Distance: %1\n" ).arg( QObject::tr( "Infinite" ) );
        else
            tag += QObject::tr( "Distance: %1\n" ).arg( QString().sprintf( "%5.2fm", (double)ImageInfo.getDistance() ) );
    }

    if ( ImageInfo.getExposureBias() ) {
        tag += QObject::tr( "Exposure bias: %1\n", QString().sprintf("%4.2f", (double)ImageInfo.getExposureBias() ) );
    }

    if ( ImageInfo.getWhitebalance() != -1 )
        tag += ExifData::white_balance_string( ImageInfo.getWhitebalance() );


    if( ImageInfo.getMeteringMode() != -1 )
        tag += ExifData::metering_mode( ImageInfo.getMeteringMode() );

    if ( ImageInfo.getExposureProgram() )
        tag += ExifData::exposure_program( ImageInfo.getExposureProgram() );
    if ( ImageInfo.getISOequivalent() )
        tag += QObject::tr( "ISO equivalent: %1\n" ).arg( QString().sprintf("%2d", ImageInfo.getISOequivalent() ) );

    tmp = ImageInfo.getUserComment();
    if ( tmp.length() )
        tag += QObject::tr( "EXIF comment: %1" ).arg( tmp );

    tag += QObject::tr( "</qt>" );



    return tag;
}

QPixmap JpegSlave::pixmap( const QString& path, int wid, int hei) {
    ExifData ImageInfo;
    /*
     */
    if ( !ImageInfo.scan( path ) || ImageInfo.isNullThumbnail() ) {
        QImage img;
        QImageIO iio( path, 0l );
        if (wid < ImageInfo.getWidth() || hei<ImageInfo.getHeight()) {
            QString str = QString( "Fast Shrink( 4 ) Scale( %1, %2, ScaleFree)" ).arg( wid ).arg( hei );
            iio.setParameters( str.latin1() );// will be strdupped anyway
        } else {
        }
        img = iio.read() ? iio.image() : QImage();
        return ThumbNailTool::scaleImage(  img, wid,hei );
    }else{
        QImage img = ImageInfo.getThumbnail();
        return ThumbNailTool::scaleImage(  img, wid,hei );
    }
}
