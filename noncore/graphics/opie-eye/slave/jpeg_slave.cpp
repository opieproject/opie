#include  "jpeg_slave.h"

#include "thumbnailtool.h"

PHUNK_VIEW_INTERFACE( "JPEG", JpegSlave )

#include <qtopia/timestring.h>
#include <qobject.h>
#include <qimage.h>

/**
	exif.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <qstring.h>
#include <qfile.h>
#include <qimage.h>

typedef enum {
    READ_EXIF = 1,
    READ_IMAGE = 2,
    READ_ALL = 3
}ReadMode_t;

//--------------------------------------------------------------------------
// This structure is used to store jpeg file sections in memory.
typedef struct {
    uchar *  Data;
    int      Type;
    unsigned Size;
}Section_t;

typedef unsigned char uchar;

typedef struct {
    unsigned short Tag;
    const char*const Desc;
}TagTable_t;

#define MAX_SECTIONS 20
#define PSEUDO_IMAGE_MARKER 0x123; // Extra value.

class ExifData {
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
    int Exif2tm(struct tm * timeptr, char * ExifTime);

public:
    ExifData();
    bool scan(const QString &);
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
};

class FatalError {
    const char* ex;
public:
    FatalError(const char* s) { ex = s; }
    void debug_print() const { qWarning("exception: %s", ex ); }
};



static unsigned char * LastExifRefd;
static int ExifSettingsLength;
static double FocalplaneXRes;
static double FocalplaneUnits;
static int MotorolaOrder = 0;
static int SectionsRead;
//static int HaveAll;

//--------------------------------------------------------------------------
// Table of Jpeg encoding process names

#define M_SOF0  0xC0            // Start Of Frame N
#define M_SOF1  0xC1            // N indicates which compression process
#define M_SOF2  0xC2            // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5            // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            // Start Of Image (beginning of datastream)
#define M_EOI   0xD9            // End Of Image (end of datastream)
#define M_SOS   0xDA            // Start Of Scan (begins compressed data)
#define M_JFIF  0xE0            // Jfif marker
#define M_EXIF  0xE1            // Exif marker
#define M_COM   0xFE            // COMment


TagTable_t ProcessTable[] = {
    { M_SOF0,   "Baseline"},
    { M_SOF1,   "Extended sequential"},
    { M_SOF2,   "Progressive"},
    { M_SOF3,   "Lossless"},
    { M_SOF5,   "Differential sequential"},
    { M_SOF6,   "Differential progressive"},
    { M_SOF7,   "Differential lossless"},
    { M_SOF9,   "Extended sequential, arithmetic coding"},
    { M_SOF10,  "Progressive, arithmetic coding"},
    { M_SOF11,  "Lossless, arithmetic coding"},
    { M_SOF13,  "Differential sequential, arithmetic coding"},
    { M_SOF14,  "Differential progressive, arithmetic coding"},
    { M_SOF15,  "Differential lossless, arithmetic coding"},
    { 0,        "Unknown"}
};



//--------------------------------------------------------------------------
// Describes format descriptor
static int BytesPerFormat[] = {0,1,1,2,4,8,1,1,2,4,8,4,8};
#define NUM_FORMATS 12

#define FMT_BYTE       1
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12

//--------------------------------------------------------------------------
// Describes tag values

#define TAG_EXIF_OFFSET       0x8769
#define TAG_INTEROP_OFFSET    0xa005

#define TAG_MAKE              0x010F
#define TAG_MODEL             0x0110
#define TAG_ORIENTATION       0x0112

#define TAG_EXPOSURETIME      0x829A
#define TAG_FNUMBER           0x829D

#define TAG_SHUTTERSPEED      0x9201
#define TAG_APERTURE          0x9202
#define TAG_MAXAPERTURE       0x9205
#define TAG_FOCALLENGTH       0x920A

#define TAG_DATETIME_ORIGINAL 0x9003
#define TAG_USERCOMMENT       0x9286

#define TAG_SUBJECT_DISTANCE  0x9206
#define TAG_FLASH             0x9209

#define TAG_FOCALPLANEXRES    0xa20E
#define TAG_FOCALPLANEUNITS   0xa210
#define TAG_EXIF_IMAGEWIDTH   0xA002
#define TAG_EXIF_IMAGELENGTH  0xA003

// the following is added 05-jan-2001 vcs
#define TAG_EXPOSURE_BIAS     0x9204
#define TAG_WHITEBALANCE      0x9208
#define TAG_METERING_MODE     0x9207
#define TAG_EXPOSURE_PROGRAM  0x8822
#define TAG_ISO_EQUIVALENT    0x8827
#define TAG_COMPRESSION_LEVEL 0x9102

#define TAG_THUMBNAIL_OFFSET  0x0201
#define TAG_THUMBNAIL_LENGTH  0x0202




//--------------------------------------------------------------------------
// Parse the marker stream until SOS or EOI is seen;
//--------------------------------------------------------------------------
int ExifData::ReadJpegSections (QFile & infile, ReadMode_t ReadMode)
{
    int a;

    a = infile.getch();

    if (a != 0xff || infile.getch() != M_SOI) {
        SectionsRead = 0;
        return false;
    }
    for(SectionsRead = 0; SectionsRead < MAX_SECTIONS-1; ){
        int marker = 0;
        int got;
        unsigned int ll,lh;
        unsigned int itemlen;
        uchar * Data;

        for (a=0;a<7;a++){
            marker = infile.getch();
            if (marker != 0xff) break;

            if (a >= 6){

                qWarning( "too many padding bytes" );
                return false;

            }
        }

        if (marker == 0xff){
            // 0xff is legal padding, but if we get that many, something's wrong.
            return false;
        }

        Sections[SectionsRead].Type = marker;

        // Read the length of the section.
        lh = (uchar) infile.getch();
        ll = (uchar) infile.getch();

        itemlen = (lh << 8) | ll;

        if (itemlen < 2) {
            return false;;
        }

        Sections[SectionsRead].Size = itemlen;

        Data = (uchar *)malloc(itemlen+1); // Add 1 to allow sticking a 0 at the end.
        Sections[SectionsRead].Data = Data;

        // Store first two pre-read bytes.
        Data[0] = (uchar)lh;
        Data[1] = (uchar)ll;

        got = infile.readBlock((char*)Data+2, itemlen-2); // Read the whole section.
        if (( unsigned ) got != itemlen-2){
            return false;
        }
        SectionsRead++;

        switch(marker){

            case M_SOS:   // stop before hitting compressed data
                // If reading entire image is requested, read the rest of the data.
                if (ReadMode & READ_IMAGE){
                    unsigned long size;

                    size = QMAX( 0ul, infile.size()-infile.at() );
                    Data = (uchar *)malloc(size);
                    if (Data == NULL){
                        return false;
                    }

                    got = infile.readBlock((char*)Data,  size);
                    if (( unsigned ) got != size){
                        return false;
                    }

                    Sections[SectionsRead].Data = Data;
                    Sections[SectionsRead].Size = size;
                    Sections[SectionsRead].Type = PSEUDO_IMAGE_MARKER;
                    SectionsRead ++;
                    //HaveAll = 1;
                }
                return true;

            case M_EOI:   // in case it's a tables-only JPEG stream
                qWarning( "No image in jpeg!" );
                return false;

            case M_COM: // Comment section
		// pieczy 2002-02-12
		// now the User comment goes to UserComment
		// so we can store a Comment section also in READ_EXIF mode
		process_COM(Data, itemlen);
                break;

            case M_JFIF:
                // Regular jpegs always have this tag, exif images have the exif
                // marker instead, althogh ACDsee will write images with both markers.
                // this program will re-create this marker on absence of exif marker.
                // hence no need to keep the copy from the file.
                free(Sections[--SectionsRead].Data);
                break;

            case M_EXIF:
                // Seen files from some 'U-lead' software with Vivitar scanner
                // that uses marker 31 for non exif stuff.  Thus make sure
                // it says 'Exif' in the section before treating it as exif.
                if ((ReadMode & READ_EXIF) && memcmp(Data+2, "Exif", 4) == 0){
                    process_EXIF((uchar *)Data, itemlen);
                }else{
                    // Discard this section.
                    free(Sections[--SectionsRead].Data);
                }
                break;

            case M_SOF0:
            case M_SOF1:
            case M_SOF2:
            case M_SOF3:
            case M_SOF5:
            case M_SOF6:
            case M_SOF7:
            case M_SOF9:
            case M_SOF10:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
                process_SOFn(Data, marker);
            default:
                break;
                break;
        }
    }
    return true;
}


//--------------------------------------------------------------------------
// Discard read data.
//--------------------------------------------------------------------------
void ExifData::DiscardData(void)
{
    for (int a=0; a < SectionsRead; a++)
        free(Sections[a].Data);
    SectionsRead = 0;
}

//--------------------------------------------------------------------------
// Convert a 16 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
int ExifData::Get16u(void * Short)
{
    if (MotorolaOrder){
        return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
    }else{
        return (((uchar *)Short)[1] << 8) | ((uchar *)Short)[0];
    }
}

//--------------------------------------------------------------------------
// Convert a 32 bit signed value from file's native byte order
//--------------------------------------------------------------------------
int ExifData::Get32s(void * Long)
{
    if (MotorolaOrder){
        return  ((( char *)Long)[0] << 24) | (((uchar *)Long)[1] << 16)
              | (((uchar *)Long)[2] << 8 ) | (((uchar *)Long)[3] << 0 );
    }else{
        return  ((( char *)Long)[3] << 24) | (((uchar *)Long)[2] << 16)
              | (((uchar *)Long)[1] << 8 ) | (((uchar *)Long)[0] << 0 );
    }
}

//--------------------------------------------------------------------------
// Convert a 32 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
unsigned ExifData::Get32u(void * Long)
{
    return (unsigned)Get32s(Long) & 0xffffffff;
}

//--------------------------------------------------------------------------
// Evaluate number, be it int, rational, or float from directory.
//--------------------------------------------------------------------------
double ExifData::ConvertAnyFormat(void * ValuePtr, int Format)
{
    double Value;
    Value = 0;

    switch(Format){
        case FMT_SBYTE:     Value = *(signed char *)ValuePtr;  break;
        case FMT_BYTE:      Value = *(uchar *)ValuePtr;        break;

        case FMT_USHORT:    Value = Get16u(ValuePtr);          break;

        case FMT_ULONG:     Value = Get32u(ValuePtr);          break;

        case FMT_URATIONAL:
        case FMT_SRATIONAL:
            {
                int Num,Den;
                Num = Get32s(ValuePtr);
                Den = Get32s(4+(char *)ValuePtr);
                if (Den == 0){
                    Value = 0;
                }else{
                    Value = (double)Num/Den;
                }
                break;
            }

        case FMT_SSHORT:    Value = (signed short)Get16u(ValuePtr);  break;
        case FMT_SLONG:     Value = Get32s(ValuePtr);                break;

        // Not sure if this is correct (never seen float used in Exif format)
        case FMT_SINGLE:    Value = (double)*(float *)ValuePtr;      break;
        case FMT_DOUBLE:    Value = *(double *)ValuePtr;             break;
    }
    return Value;
}

//--------------------------------------------------------------------------
// Process one of the nested EXIF directories.
//--------------------------------------------------------------------------
void ExifData::ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength)
{
    int de;
    int a;
    int NumDirEntries;
    unsigned ThumbnailOffset = 0;
    unsigned ThumbnailSize = 0;

    NumDirEntries = Get16u(DirStart);
    #define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))

    {
        unsigned char * DirEnd;
        DirEnd = DIR_ENTRY_ADDR(DirStart, NumDirEntries);
        if (DirEnd+4 > (OffsetBase+ExifLength)){
            if (DirEnd+2 == OffsetBase+ExifLength || DirEnd == OffsetBase+ExifLength){
                // Version 1.3 of jhead would truncate a bit too much.
                // This also caught later on as well.
            }else{
                // Note: Files that had thumbnails trimmed with jhead 1.3 or earlier
                // might trigger this.
                return;
            }
        }
        if (DirEnd < LastExifRefd) LastExifRefd = DirEnd;
    }

    for (de=0;de<NumDirEntries;de++){
        int Tag, Format, Components;
        unsigned char * ValuePtr;
        int ByteCount;
        char * DirEntry;
        DirEntry = (char *)DIR_ENTRY_ADDR(DirStart, de);

        Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);

        if ((Format-1) >= NUM_FORMATS) {
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            return;
        }

        ByteCount = Components * BytesPerFormat[Format];

        if (ByteCount > 4){
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8);
            // If its bigger than 4 bytes, the dir entry contains an offset.
            if (OffsetVal+ByteCount > ExifLength){
                // Bogus pointer offset and / or bytecount value
                //printf("Offset %d bytes %d ExifLen %d\n",OffsetVal, ByteCount, ExifLength);

                return;
            }
            ValuePtr = OffsetBase+OffsetVal;
        }else{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = (unsigned char *)DirEntry+8;
        }

        if (LastExifRefd < ValuePtr+ByteCount){
            // Keep track of last byte in the exif header that was actually referenced.
            // That way, we know where the discardable thumbnail data begins.
            LastExifRefd = ValuePtr+ByteCount;
        }

        // Extract useful components of tag
        switch(Tag){

            case TAG_MAKE:
                ExifData::CameraMake = QString((char*)ValuePtr);
                break;

            case TAG_MODEL:
                ExifData::CameraModel = QString((char*)ValuePtr);
		break;

            case TAG_ORIENTATION:
                Orientation = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_DATETIME_ORIGINAL:
		DateTime = QString((char*)ValuePtr);
                break;

            case TAG_USERCOMMENT:
                // Olympus has this padded with trailing spaces.  Remove these first.
                for (a=ByteCount;;){
                    a--;
                    if ((ValuePtr)[a] == ' '){
                        (ValuePtr)[a] = '\0';
                    }else{
                        break;
                    }
                    if (a == 0) break;
                }

                // Copy the comment
                if (memcmp(ValuePtr, "ASCII",5) == 0){
                    for (a=5;a<10;a++){
                        int c;
                        c = (ValuePtr)[a];
                        if (c != '\0' && c != ' '){
                            //strncpy(ImageInfo.Comments, (const char*)(a+ValuePtr), 199);
                            UserComment.sprintf("%s", (const char*)(a+ValuePtr));
                            break;
                        }
                    }
                }else{
                    //strncpy(ImageInfo.Comments, (const char*)ValuePtr, 199);
                    UserComment.sprintf("%s", (const char*)ValuePtr);
                }
                break;

            case TAG_FNUMBER:
                // Simplest way of expressing aperture, so I trust it the most.
                // (overwrite previously computd value if there is one)
		ExifData::ApertureFNumber = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_APERTURE:
            case TAG_MAXAPERTURE:
                // More relevant info always comes earlier, so only use this field if we don't
                // have appropriate aperture information yet.
                if (ExifData::ApertureFNumber == 0){
                    ExifData::ApertureFNumber
                        = (float)exp(ConvertAnyFormat(ValuePtr, Format)*log(2)*0.5);
                }
                break;

            case TAG_FOCALLENGTH:
                // Nice digital cameras actually save the focal length as a function
                // of how farthey are zoomed in.
                ExifData::FocalLength = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_SUBJECT_DISTANCE:
                // Inidcates the distacne the autofocus camera is focused to.
                // Tends to be less accurate as distance increases.
                ExifData::Distance = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_EXPOSURETIME:
                // Simplest way of expressing exposure time, so I trust it most.
                // (overwrite previously computd value if there is one)
                ExifData::ExposureTime = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_SHUTTERSPEED:
                // More complicated way of expressing exposure time, so only use
                // this value if we don't already have it from somewhere else.
                if (ExifData::ExposureTime == 0){
                    ExifData::ExposureTime
                        = (float)(1/exp(ConvertAnyFormat(ValuePtr, Format)*log(2)));
                }
                break;

            case TAG_FLASH:
                if (ConvertAnyFormat(ValuePtr, Format)){
                    ExifData::FlashUsed = 1;
                }
                break;

            case TAG_EXIF_IMAGELENGTH:
                ExifImageLength = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_EXIF_IMAGEWIDTH:
                ExifImageWidth = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_FOCALPLANEXRES:
                FocalplaneXRes = ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_FOCALPLANEUNITS:
                switch((int)ConvertAnyFormat(ValuePtr, Format)){
                    case 1: FocalplaneUnits = 25.4; break; // inch
                    case 2:
                        // According to the information I was using, 2 means meters.
                        // But looking at the Cannon powershot's files, inches is the only
                        // sensible value.
                        FocalplaneUnits = 25.4;
                        break;

                    case 3: FocalplaneUnits = 10;   break;  // centimeter
                    case 4: FocalplaneUnits = 1;    break;  // milimeter
                    case 5: FocalplaneUnits = .001; break;  // micrometer
                }
                break;

                // Remaining cases contributed by: Volker C. Schoech (schoech@gmx.de)

            case TAG_EXPOSURE_BIAS:
	    	ExifData::ExposureBias = (float)ConvertAnyFormat(ValuePtr, Format);
		break;

            case TAG_WHITEBALANCE:
                 ExifData::Whitebalance = (int)ConvertAnyFormat(ValuePtr, Format);
		 break;

            case TAG_METERING_MODE:
                ExifData::MeteringMode = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_EXPOSURE_PROGRAM:
                ExifData::ExposureProgram = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_ISO_EQUIVALENT:
                ExifData::ISOequivalent = (int)ConvertAnyFormat(ValuePtr, Format);
                if ( ExifData::ISOequivalent < 50 ) ExifData::ISOequivalent *= 200;
                break;

            case TAG_COMPRESSION_LEVEL:
                ExifData::CompressionLevel = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_THUMBNAIL_OFFSET:
                ThumbnailOffset = (unsigned)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_THUMBNAIL_LENGTH:
                ThumbnailSize = (unsigned)ConvertAnyFormat(ValuePtr, Format);
                break;

        }

        if (Tag == TAG_EXIF_OFFSET || Tag == TAG_INTEROP_OFFSET){
            unsigned char * SubdirStart;
            SubdirStart = OffsetBase + Get32u(ValuePtr);
            if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength){
                return;
            }
            ProcessExifDir(SubdirStart, OffsetBase, ExifLength);
            continue;
        }
    }

    {
        // In addition to linking to subdirectories via exif tags,
        // there's also a potential link to another directory at the end of each
        // directory.  this has got to be the result of a comitee!
        unsigned char * SubdirStart;
        unsigned Offset;

        if (DIR_ENTRY_ADDR(DirStart, NumDirEntries) + 4 <= OffsetBase+ExifLength){
            Offset = Get32u(DIR_ENTRY_ADDR(DirStart, NumDirEntries));
	    // There is at least one jpeg from an HP camera having an Offset of almost MAXUINT.
	    // Adding OffsetBase to it produces an overflow, so compare with ExifLength here.
	    // See http://bugs.kde.org/show_bug.cgi?id=54542
	    if (Offset && Offset < ExifLength){
                SubdirStart = OffsetBase + Offset;
                if (SubdirStart > OffsetBase+ExifLength){
                    if (SubdirStart < OffsetBase+ExifLength+20){
                        // Jhead 1.3 or earlier would crop the whole directory!
                        // As Jhead produces this form of format incorrectness,
                        // I'll just let it pass silently
                        qWarning( "Thumbnail removed with Jhead 1.3 or earlier" );
                    }else{
                        return;
                    }
                }else{
                    if (SubdirStart <= OffsetBase+ExifLength){
                        ProcessExifDir(SubdirStart, OffsetBase, ExifLength);
                    }
                }
            }
        }else{
            // The exif header ends before the last next directory pointer.
        }
    }

    if (ThumbnailSize && ThumbnailOffset){
        if (ThumbnailSize + ThumbnailOffset <= ExifLength){
            // The thumbnail pointer appears to be valid.  Store it.
	    Thumbnail.loadFromData(OffsetBase + ThumbnailOffset, ThumbnailSize, "JPEG");
        }
    }
}

//--------------------------------------------------------------------------
// Process a COM marker.  We want to leave the bytes unchanged.  The
// progam that displays this text may decide to remove blanks, convert
// newlines, or otherwise modify the text.  In particular we want to be
// safe for passing utf-8 text.
//--------------------------------------------------------------------------
void ExifData::process_COM (const uchar * Data, int length)
{
    QChar ch;
    int a;

    for (a=2;a<length;a++){
        ch = Data[a];
        if (ch == '\000') continue;                     // Remove nulls
        Comment.append(ch);
    }
}


//--------------------------------------------------------------------------
// Process a SOFn marker.  This is useful for the image dimensions
//--------------------------------------------------------------------------
void ExifData::process_SOFn (const uchar * Data, int marker)
{
    int data_precision, num_components;

    data_precision = Data[2];
    ExifData::Height = Get16m(Data+3);
    ExifData::Width = Get16m(Data+5);
    num_components = Data[7];

    if (num_components == 3){
        ExifData::IsColor = 1;
    }else{
        ExifData::IsColor = 0;
    }

    ExifData::Process = marker;

}

//--------------------------------------------------------------------------
// Get 16 bits motorola order (always) for jpeg header stuff.
//--------------------------------------------------------------------------
int ExifData::Get16m(const void * Short)
{
    return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
}


//--------------------------------------------------------------------------
// Process a EXIF marker
// Describes all the drivel that most digital cameras include...
//--------------------------------------------------------------------------
void ExifData::process_EXIF(unsigned char * CharBuf, unsigned int length)
{
    ExifData::FlashUsed = 0; // If it s from a digicam, and it used flash, it says so.

    FocalplaneXRes = 0;
    FocalplaneUnits = 0;
    ExifImageWidth = 0;
    ExifImageLength = 0;

    {   // Check the EXIF header component
        static const uchar ExifHeader[] = "Exif\0\0";
        if (memcmp(CharBuf+2, ExifHeader,6)){
            return;
        }
    }

    if (memcmp(CharBuf+8,"II",2) == 0){
        // printf("Exif section in Intel order\n");
        MotorolaOrder = 0;
    }else{
        if (memcmp(CharBuf+8,"MM",2) == 0){
            // printf("Exif section in Motorola order\n");
            MotorolaOrder = 1;
        }else{
            return;
        }
    }

    // Check the next two values for correctness.
    if (Get16u(CharBuf+10) != 0x2a
      || Get32u(CharBuf+12) != 0x08){
        return;
    }

    LastExifRefd = CharBuf;

    // First directory starts 16 bytes in.  Offsets start at 8 bytes in.
    ProcessExifDir(CharBuf+16, CharBuf+8, length-6);

    // This is how far the interesting (non thumbnail) part of the exif went.
    ExifSettingsLength = LastExifRefd - CharBuf;

    // Compute the CCD width, in milimeters.
    if (FocalplaneXRes != 0){
        ExifData::CCDWidth = (float)(ExifImageWidth * FocalplaneUnits / FocalplaneXRes);
    }
}

//--------------------------------------------------------------------------
// Convert exif time to Unix time structure
//--------------------------------------------------------------------------
int ExifData::Exif2tm(struct tm * timeptr, char * ExifTime)
{
    int a;

    timeptr->tm_wday = -1;

    // Check for format: YYYY:MM:DD HH:MM:SS format.
    a = sscanf(ExifTime, "%d:%d:%d %d:%d:%d",
            &timeptr->tm_year, &timeptr->tm_mon, &timeptr->tm_mday,
            &timeptr->tm_hour, &timeptr->tm_min, &timeptr->tm_sec);

    if (a == 6){
        timeptr->tm_isdst = -1;
        timeptr->tm_mon -= 1;      // Adjust for unix zero-based months
        timeptr->tm_year -= 1900;  // Adjust for year starting at 1900
        return true; // worked.
    }

    return false; // Wasn't in Exif date format.
}

//--------------------------------------------------------------------------
// Contructor for initialising
//--------------------------------------------------------------------------
ExifData::ExifData()
{
    ExifData::Whitebalance = -1;
    ExifData::MeteringMode = -1;
    ExifData::FlashUsed = -1;
    Orientation = 0;
    Height = 0;
    Width = 0;
    IsColor = 0;
    Process = 0;
    FocalLength = 0;
    ExposureTime = 0;
    ApertureFNumber = 0;
    Distance = 0;
    CCDWidth = 0;
    ExposureBias = 0;
    ExposureProgram = 0;
    ISOequivalent = 0;
    CompressionLevel = 0;
}

//--------------------------------------------------------------------------
// process a EXIF jpeg file
//--------------------------------------------------------------------------
bool ExifData::scan(const QString & path)
{
    int ret;

    QFile f(path);
    f.open(IO_ReadOnly);

    // Scan the JPEG headers.
    ret = ReadJpegSections(f, READ_EXIF);

    if (ret == false){
        qWarning(  "Not JPEG file!" );
        DiscardData();
        f.close();
        return false;
    }
    f.close();
    DiscardData();

    //now make the strings clean,
    // for exmaple my Casio is a "QV-4000   "
    CameraMake = CameraMake.stripWhiteSpace();
    CameraModel = CameraModel.stripWhiteSpace();
    UserComment = UserComment.stripWhiteSpace();
    Comment = Comment.stripWhiteSpace();
    return true;
}

//--------------------------------------------------------------------------
// Does the embedded thumbnail match the jpeg image?
//--------------------------------------------------------------------------
#ifndef JPEG_TOL
#define JPEG_TOL 0.02
#endif
bool ExifData::isThumbnailSane() {
  if (Thumbnail.isNull()) return false;

  // check whether thumbnail dimensions match the image
  // not foolproof, but catches some altered images (jpegtran -rotate)
  if (ExifImageLength != 0 && ExifImageLength != Height) return false;
  if (ExifImageWidth != 0 && ExifImageWidth != Width) return false;
  if (Thumbnail.width() == 0 || Thumbnail.height() == 0) return false;
  if (Height == 0 || Width == 0) return false;
  double d = (double)Height/Width*Thumbnail.width()/Thumbnail.height();
  return (1-JPEG_TOL < d) && (d < 1+JPEG_TOL);
}



static QImage flip_image( const QImage& img );
static QImage rotate_90( const QImage& img );
static QImage rotate_180( const QImage& );
static QImage rotate_270( const QImage& );

//--------------------------------------------------------------------------
// return a thumbnail that respects the orientation flag
// only if it seems sane
//--------------------------------------------------------------------------
QImage ExifData::getThumbnail() {
  if (!isThumbnailSane()) return NULL;
  if (!Orientation || Orientation == 1) return Thumbnail;

  // now fix orientation

  QImage dest = Thumbnail;
  switch (Orientation) {  // notice intentional fallthroughs
    case 2: dest = flip_image( dest ); break;
    case 4: dest = flip_image( dest );
    case 3: dest =rotate_180( dest ); break;
    case 5: dest = flip_image( dest );
    case 6: dest = rotate_90( dest ); break;
    case 7: dest = flip_image( dest );
    case 8: dest = rotate_270( dest ); break;
    default: break; // should never happen
  }
  return dest;
}


/*
 *
 */
static QImage flip_image( const QImage& img ) {
    return img.mirror( TRUE, FALSE );
}


static QImage dest;
static int x, y;
static unsigned int  *srcData, *destData; // we're not threaded anyway
static unsigned char *srcData8, *destData8; // 8 bit is char
static unsigned int *srcTable,  *destTable; // destination table


static QImage rotate_90_8( const QImage &img ) {
    dest.create(img.height(), img.width(), img.depth());
    dest.setNumColors(img.numColors());
    srcTable = (unsigned int *)img.colorTable();
    destTable = (unsigned int *)dest.colorTable();
    for ( x=0; x < img.numColors(); ++x )
        destTable[x] = srcTable[x];
    for ( y=0; y < img.height(); ++y ){
        srcData8 = (unsigned char *)img.scanLine(y);
        for ( x=0; x < img.width(); ++x ){
            destData8 = (unsigned char *)dest.scanLine(x);
            destData8[img.height()-y-1] = srcData8[x];
        }
    }
    return dest;
}

static QImage rotate_90_all(  const QImage& img ) {
    dest.create(img.height(), img.width(), img.depth());
    for ( y=0; y < img.height(); ++y )    {
        srcData = (unsigned int *)img.scanLine(y);
        for ( x=0; x < img.width(); ++x ) {
            destData = (unsigned int *)dest.scanLine(x);
            destData[img.height()-y-1] = srcData[x];
        }
    }

    return dest;
}


static QImage  rotate_90( const QImage & img ) {
    if ( img.depth() > 8)
        return rotate_90_all( img );
    else
        return rotate_90_8( img );
}

static QImage rotate_180_all( const QImage& img ) {
    dest.create(img.width(), img.height(), img.depth());
    for ( y=0; y < img.height(); ++y ){
        srcData = (unsigned int *)img.scanLine(y);
        destData = (unsigned int *)dest.scanLine(img.height()-y-1);
        for ( x=0; x < img.width(); ++x )
            destData[img.width()-x-1] = srcData[x];
    }
    return dest;
}

static QImage rotate_180_8( const QImage& img ) {
    dest.create(img.width(), img.height(), img.depth());
    dest.setNumColors(img.numColors());
    srcTable = (unsigned int *)img.colorTable();
    destTable = (unsigned int *)dest.colorTable();
    for ( x=0; x < img.numColors(); ++x )
        destTable[x] = srcTable[x];
    for ( y=0; y < img.height(); ++y ){
        srcData8 = (unsigned char *)img.scanLine(y);
        destData8 = (unsigned char *)dest.scanLine(img.height()-y-1);
        for ( x=0; x < img.width(); ++x )
            destData8[img.width()-x-1] = srcData8[x];
    }
    return dest;
}

static QImage rotate_180( const QImage& img ) {
    if ( img.depth() > 8 )
        return rotate_180_all( img );
    else
        return rotate_180_8( img );
}


static QImage rotate_270_8( const QImage& img ) {
    dest.create(img.height(), img.width(), img.depth());
    dest.setNumColors(img.numColors());
    srcTable = (unsigned int *)img.colorTable();
    destTable = (unsigned int *)dest.colorTable();
    for ( x=0; x < img.numColors(); ++x )
        destTable[x] = srcTable[x];
    for ( y=0; y < img.height(); ++y ){
        srcData8 = (unsigned char *)img.scanLine(y);
        for ( x=0; x < img.width(); ++x ){
            destData8 = (unsigned char *)dest.scanLine(img.width()-x-1);
            destData8[y] = srcData8[x];
        }
    }

    return dest;
}

static QImage rotate_270_all( const QImage& img ) {
    dest.create(img.height(), img.width(), img.depth());
    for ( y=0; y < img.height(); ++y ){
        srcData = (unsigned int *)img.scanLine(y);
        for ( x=0; x < img.width(); ++x ){
            destData = (unsigned int *)dest.scanLine(img.width()-x-1);
            destData[y] = srcData[x];
        }
    }
    return dest;
}

static QImage rotate_270( const QImage& img ) {
    if (  img.depth() > 8 )
        return rotate_270_all( img );
    else
        return rotate_270_8( img );
}


static QString color_mode_to_string( bool b ) {
    return b ? QObject::tr( "Colormode: Color\n" ) : QObject::tr( "Colormode: Black and white\n" );
}

static QString compression_to_string( int level ) {
    QString str;
    switch( level ) {
    case 1:
        str = QObject::tr( "Basic" );
        break;
    case 2:
        str = QObject::tr( "Normal" );
        break;
    case 4:
        str = QObject::tr( "Fine" );
        break;
    default:
        str = QObject::tr( "Unknown" );

    }
    return QObject::tr("Quality: %1\n").arg(str);
}


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

static QString white_balance_string( int i ) {
    QString balance;
    switch ( i ) {
    case 0:
        balance = QObject::tr( "Unknown" );
        break;
    case 1:
        balance = QObject::tr( "Daylight" );
        break;
    case 2:
        balance = QObject::tr( "Fluorescent" );
        break;
    case 3:
        balance = QObject::tr( "Tungsten" );
        break;
    case 17:
        balance = QObject::tr( "Standard light A" );
        break;
    case 18:
        balance = QObject::tr( "Standard light B" );
        break;
    case 19:
        balance = QObject::tr( "Standard light C" );
        break;
    case 20:
        balance = QObject::tr( "D55" );
        break;
    case 21:
        balance = QObject::tr( "D65" );
        break;
    case 22:
        balance = QObject::tr( "D75" );
        break;
    case 255:
        balance = QObject::tr( "Other" );
        break;
    default:
        balance = QObject::tr( "Unknown" );
    }
    return QObject::tr( "White Balance: %1\n" ).arg( balance );

}


static QString metering_mode( int i) {
    QString meter;
    switch( i ) {
    case 0:
        meter = QObject::tr( "Unknown" );
        break;
    case 1:
        meter = QObject::tr( "Average" );
        break;
    case 2:
        meter = QObject::tr( "Center weighted average" );
        break;
    case 3:
        meter = QObject::tr( "Spot" );
        break;
    case 4:
        meter = QObject::tr( "MultiSpot" );
        break;
    case 5:
        meter = QObject::tr( "Pattern" );
        break;
    case 6:
        meter = QObject::tr( "Partial" );
        break;
    case 255:
        meter = QObject::tr( "Other" );
        break;
    default:
        meter = QObject::tr( "Unknown" );
    }

    return QObject::tr( "Metering Mode: %1\n" ).arg( meter );
}


static QString exposure_program( int i ) {
    QString exp;
    switch( i ) {
    case 0:
        exp = QObject::tr( "Not defined" );
        break;
    case 1:
        exp = QObject::tr( "Manual" );
        break;
    case 2:
        exp = QObject::tr( "Normal progam" );
        break;
    case 3:
        exp = QObject::tr( "Aperture priority" );
        break;
    case 4:
        exp = QObject::tr( "Shutter priority" );
        break;
    case 5:
        exp = QObject::tr( "Creative progam\n(biased toward fast shutter speed" );
        break;
    case 6:
        exp = QObject::tr( "Action progam\n(biased toward fast shutter speed)" );
        break;
    case 7:
        exp = QObject::tr( "Portrait mode\n(for closeup photos with the background out of focus)" );
        break;
    case 8:
        exp = QObject::tr( "Landscape mode\n(for landscape photos with the background in focus)" );
        break;
    default:
        exp = QObject::tr( "Unknown" );
    }

    return QObject::tr( "Exposure Program: %1\n" ).arg( exp );
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
    tag  = QObject::tr( "<qt>Comment: %1\n" ).arg( ImageInfo.getComment() );
    {
// ODP fixme
        QString timestring = TimeString::dateString( parseDateTime(  ImageInfo.getDateTime() ), FALSE );
        tag += QObject::tr( "Date/Time: %1\n" ).arg( timestring );
    }
    tag += QObject::tr( "Dimensions: %1x%2\n" ).arg(ImageInfo.getWidth())
           .arg(ImageInfo.getHeight() );

    tag += color_mode_to_string(  ImageInfo.getIsColor() );

    tag += compression_to_string( ImageInfo.getCompressionLevel() );
    tag += QObject::tr( "</qt>" );

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
    tag   = QObject::tr( "Comment: %1\n" ).arg( ImageInfo.getComment() );

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

    tag += color_mode_to_string(  ImageInfo.getIsColor() );

    tag += compression_to_string( ImageInfo.getCompressionLevel() );
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
        tag += white_balance_string( ImageInfo.getWhitebalance() );


    if( ImageInfo.getMeteringMode() != -1 )
        tag += metering_mode( ImageInfo.getMeteringMode() );

    if ( ImageInfo.getExposureProgram() )
        tag += exposure_program( ImageInfo.getExposureProgram() );
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
    if ( !ImageInfo.scan( path ) || ImageInfo.isNullThumbnail() ) {
        QImage img;
        QImageIO iio( path, 0l );
        QString str = QString( "Fast Shrink( 4 ) Scale( %1, %2, ScaleFree)" ).arg( wid ).arg( hei );
        iio.setParameters( str.latin1() );// will be strdupped anyway
        img = iio.read() ? iio.image() : QImage();
        return ThumbNailTool::scaleImage(  img, wid,hei );
    }else{
        QImage img = ImageInfo.getThumbnail();
        return ThumbNailTool::scaleImage(  img, wid,hei );
    }
}
