#ifndef _OPIE_EXIF_H
#define _OPIE_EXIF_H

#include <qt.h>
#include <qstring.h>
#include <qimage.h>
#include <qfile.h>

#include <time.h>

namespace Opie { namespace MM {

#ifndef uchar
typedef unsigned char uchar;
#endif

//#define MAX_SECTIONS 20
//#define PSEUDO_IMAGE_MARKER 0x123; // Extra value.

//! Class for reading exif data from images
/*!
 * This class is mostly used inside OImageScrollView for testing jpegs headers for a faster
 * loading and scaling. It is taken from libexif and converted into an C++ structure.
 *
 * \see OImageScrollView
 * \since 1.2
 */
class ExifData {
public:
    enum ReadMode_t {
        READ_EXIF = 1,
        READ_IMAGE = 2,
        READ_ALL = 3
    };

    //--------------------------------------------------------------------------
    // This structure is used to store jpeg file sections in memory.
    struct Section_t {
        uchar *  Data;
        int      Type;
        unsigned Size;
    };

    struct TagTable_t {
        unsigned short Tag;
        const char*const Desc;
    };

private:
    static const int MAX_SECTIONS=20;
    static const unsigned int PSEUDO_IMAGE_MARKER=0x123;
    Section_t Sections[MAX_SECTIONS];

    QString CameraMake;
    QString CameraModel;
    QString DateTime;
    int   Orientation;
    int   Height, Width;
    int   ExifImageLength, ExifImageWidth;
    int   IsColor;
    int   Process;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    int    Whitebalance;
    int    MeteringMode;
    float CCDWidth;
    float ExposureBias;
    int   ExposureProgram;
    int   ISOequivalent;
    int   CompressionLevel;
    QString UserComment;
    QString Comment;
    QImage Thumbnail;

    unsigned char * LastExifRefd;
    int ExifSettingsLength;
    double FocalplaneXRes;
    double FocalplaneUnits;
    int MotorolaOrder;
    int SectionsRead;

    int ReadJpegSections (QFile & infile, ReadMode_t ReadMode);
    void DiscardData(void);
    int Get16u(void * Short);
    int Get32s(void * Long);
    unsigned Get32u(void * Long);
    double ConvertAnyFormat(void * ValuePtr, int Format);
    void ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength);
    void process_COM (const uchar * Data, int length);
    void process_SOFn (const uchar * Data, int marker);
    int Get16m(const void * Short);
    void process_EXIF(unsigned char * CharBuf, unsigned int length);
    int Exif2tm(struct ::tm * timeptr, char * ExifTime);

public:
    //! Contructor for initialising
    ExifData();
    //! destructor
    virtual ~ExifData();
    //! scan a given file
    /*!
     * try to scan the EXIF data of a image file
     * \param aFile the file to scan
     * \return true if success, otherwise false
     */
    bool scan(const QString &aFile);
    QString getCameraMake() { return CameraMake; }
    QString getCameraModel() { return CameraModel; }
    QString getDateTime() { return DateTime; }
    int getOrientation() { return Orientation; }
    int getHeight() { return Height; }
    int getWidth() { return Width; }
    int getIsColor() { return IsColor; }
    int getProcess() { return Process; }
    int getFlashUsed() { return FlashUsed; }
    float getFocalLength() { return FocalLength; }
    float getExposureTime() { return ExposureTime; }
    float getApertureFNumber() { return ApertureFNumber; }
    float getDistance() { return Distance; }
    int getWhitebalance() { return Whitebalance; }
    int getMeteringMode() { return MeteringMode; }
    float getCCDWidth() { return CCDWidth; }
    float getExposureBias() { return ExposureBias; }
    int getExposureProgram() { return ExposureProgram; }
    int getISOequivalent() { return ISOequivalent; }
    int getCompressionLevel() { return CompressionLevel; }
    QString getUserComment() { return UserComment; }
    QString getComment() { return Comment; }
    QImage getThumbnail();
    bool isThumbnailSane();
    bool isNullThumbnail() { return !isThumbnailSane(); }

    // some helpers
    static QString color_mode_to_string( bool b );
    static QString compression_to_string( int level );
    static QString white_balance_string( int i );
    static QString metering_mode( int i);
    static QString exposure_program( int i );
};

}
}
#endif
