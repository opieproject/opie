#include "png_slave.h"

#include "thumbnailtool.h"

#include <qobject.h>
#include <qfile.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstring.h>

/*
 * GPLv2 from kfile plugin
 */
PHUNK_VIEW_INTERFACE( "PNG", PNGSlave );

#define CHUNK_SIZE(data, index) ((data[index  ]<<24) + (data[index+1]<<16) + \
                                 (data[index+2]<< 8) +  data[index+3])
#define CHUNK_TYPE(data, index)  &data[index+4]
#define CHUNK_HEADER_SIZE 12
#define CHUNK_DATA(data, index, offset) data[8+index+offset]

/* TRANSLATOR QObject */

// known translations for common png keys
static const char* knownTranslations[]
#ifdef __GNUC__
__attribute__((unused))
#endif
 = {
  QT_TR_NOOP("Title"),
  QT_TR_NOOP("Author"),
  QT_TR_NOOP("Description"),
  QT_TR_NOOP("Copyright"),
  QT_TR_NOOP("Creation Time"),
  QT_TR_NOOP("Software"),
  QT_TR_NOOP("Disclaimer"),
  QT_TR_NOOP("Warning"),
  QT_TR_NOOP("Source"),
  QT_TR_NOOP("Comment")
};

// and for the colors
static const char* colors[] = {
  QT_TR_NOOP("Grayscale"),
  QT_TR_NOOP("Unknown"),
  QT_TR_NOOP("RGB"),
  QT_TR_NOOP("Palette"),
  QT_TR_NOOP("Grayscale/Alpha"),
  QT_TR_NOOP("Unknown"),
  QT_TR_NOOP("RGB/Alpha")
};

  // and compressions
static const char* compressions[] =
{
  QT_TR_NOOP("Deflate")
};

  // interlaced modes
static const char* interlaceModes[] = {
  QT_TR_NOOP("None"),
  QT_TR_NOOP("Adam7")
};


static void read_comment( const QString& inf,
                          bool readComments,  QString& str ) {
    QFile f(inf);
    f.open(IO_ReadOnly);

    if (f.size() < 26) return;
    // the technical group will be read from the first 26 bytes. If the file
    // is smaller, we can't even read this.

    uchar *data = new uchar[f.size()+1];
    f.readBlock(reinterpret_cast<char*>(data), f.size());
    data[f.size()]='\n';

    // find the start
    if (data[0] == 137 && data[1] == 80 && data[2] == 78 && data[3] == 71 &&
        data[4] ==  13 && data[5] == 10 && data[6] == 26 && data[7] == 10 )
    {
        // ok
        // the IHDR chunk should be the first
        if (!strncmp((char*)&data[12], "IHDR", 4))
        {
            // we found it, get the dimensions
            ulong x,y;
            x = (data[16]<<24) + (data[17]<<16) + (data[18]<<8) + data[19];
            y = (data[20]<<24) + (data[21]<<16) + (data[22]<<8) + data[23];

            uint type = data[25];
            uint bpp =  data[24];

            // the bpp are only per channel, so we need to multiply the with
            // the channel count
            switch (type)
            {
                case 0: break;           // Grayscale
                case 2: bpp *= 3; break; // RGB
                case 3: break;           // palette
                case 4: bpp *= 2; break; // grayscale w. alpha
                case 6: bpp *= 4; break; // RGBA

                default: // we don't get any sensible value here
                    bpp = 0;
            }


            str  = QObject::tr("Dimensions: %1x%2\n" ).arg(x).arg(y);
            str += QObject::tr("Depth: %1\n" ).arg(bpp);
            str += QObject::tr("ColorMode: %1\n").arg(
                       (type < sizeof(colors)/sizeof(colors[0]))
                       ? QObject::tr(colors[data[25]]) : QObject::tr("Unknown") );

            str += QObject::tr("Compression: %1\n").arg(
                       (data[26] < sizeof(compressions)/sizeof(compressions[0]))
                       ? QObject::tr(compressions[data[26]]) : QObject::tr("Unknown") );

            str += QObject::tr("InterlaceMode: %1\n" ).arg(
                       (data[28] < sizeof(interlaceModes)/sizeof(interlaceModes[0]))
                       ? QObject::tr(interlaceModes[data[28]]) : QObject::tr("Unknown"));
        }

        if ( readComments ) {
            uint index = 8;
            index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;

            while(index<f.size()-12)
            {
                while (index < f.size() - 12 &&
                       strncmp((char*)CHUNK_TYPE(data,index), "tEXt", 4))
                {
                    if (!strncmp((char*)CHUNK_TYPE(data,index), "IEND", 4))
                        goto end;

                    index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
                }

                if (index < f.size() - 12)
                {
                    // we found a tEXt field
                    // get the key, it's a null terminated string at the
                    //  chunk start

                    uchar* key = &CHUNK_DATA(data,index,0);

                    int keysize=0;
                    for (;key[keysize]!=0; keysize++)
                        // look if we reached the end of the file
                        // (it might be corrupted)
                        if (8+index+keysize>=f.size())
                            goto end;

                    // the text comes after the key, but isn't null terminated
                    uchar* text = &CHUNK_DATA(data,index, keysize+1);
                    uint textsize = CHUNK_SIZE(data, index)-keysize-1;

                    // security check, also considering overflow wraparound from the addition --
                    // we may endup with a /smaller/ index if we wrap all the way around
                    uint firstIndex       = (uint)(text - data);
                    uint onePastLastIndex = firstIndex + textsize;

                    if ( onePastLastIndex > f.size() || onePastLastIndex <= firstIndex)
                        goto end;

                    QByteArray arr(textsize);
                    arr = QByteArray(textsize).duplicate((const char*)text,
                                                         textsize);
                    str += QObject::tr(
                               QString(reinterpret_cast<char*>(key)),
                               QString(arr) );

                    index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
                }
            }
        }
    }
end:
    delete[] data;

}


PNGSlave::PNGSlave()
    : SlaveInterface("png")
{
}
PNGSlave::~PNGSlave() {
}
QString PNGSlave::iconViewName( const QString& path) {
    QString str;
    read_comment( path, false, str );
    return str;
}

QString PNGSlave::fullImageInfo( const QString& path) {
    QString str = "<qt>";
    read_comment( path, true, str );
    str += "</qt>";
    return str;
}


QPixmap PNGSlave::pixmap( const QString& path, int  width, int height) {
    QImage img; img.load( path );
    if ( img.isNull() )
        return QPixmap();
    else
        return ThumbNailTool::scaleImage( img, width,height );
}
