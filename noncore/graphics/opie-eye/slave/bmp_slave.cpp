#include  "bmp_slave.h"

#include "thumbnailtool.h"

#include <qimage.h>
#include <qobject.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qstring.h>

PHUNK_VIEW_INTERFACE( "Bmp", BmpSlave );
namespace {

    struct pBmpHeader {
        // file header of bmp
        char type[2]; // must be "BM" otherwise it is no bmp
        Q_INT32 hSize;
        Q_INT32 reserved1,reserved2;

        Q_INT16  Planes;
        Q_INT16  BitCount;
        Q_INT32  Size;
        Q_INT32  Width;
        Q_INT32  Height;
        Q_INT32  Compression;
        Q_INT32  SizeImage;
        Q_INT32  XPerMeter;
        Q_INT32  YPerMeter;
        Q_INT32  ClrUsed;
        Q_INT32  ClrImportant;
    };

    class BmpHeader {
    protected:
        void read_data();
        QString _name;
        QFile _inputfile;
        pBmpHeader m_Header;

        static const int OLD = 12;
        static const int WIN = 40;
        static const int OS2 = 64;
        static const int RGB = 0;
        static const int RLE8 = 1;
        static const int RLE4 = 2;

    public:
        BmpHeader(const QString&fname);
        virtual ~BmpHeader();
        bool isBmp()const{return qstrncmp(m_Header.type,"BM",2)==0;}
        bool isCompressed()const{return m_Header.Compression != 0;}
        QSize imageSize(){return QSize(m_Header.Width,m_Header.Height);}
        QString imageCompression()const;
        int bitsPixel()const{return m_Header.BitCount;}
        int Size()const{return m_Header.hSize;}
        int compressedSize()const{return m_Header.SizeImage;}
        int ColorsUsed()const{return m_Header.ClrUsed;}
        int XPix()const{return m_Header.XPerMeter;}
        int YPix()const{return m_Header.YPerMeter;}
    };

    QString BmpHeader::imageCompression()const
    {
        switch (m_Header.Compression) {
            case RLE8:
                return "8Bit RLE Encoding";
                break;
            case RLE4:
                return "4Bit RLE Encoding";
                break;
            case RGB:
            default:
                return "No encoding";
        }
    }

    BmpHeader::BmpHeader(const QString&fname)
        : _name(fname),_inputfile(_name)
    {
        read_data();
    }

    void BmpHeader::read_data() {
        memset(&m_Header,0,sizeof(pBmpHeader));
        _inputfile.open(IO_Raw|IO_ReadOnly);
        if (!_inputfile.isOpen()) {
            return;
        }
        QDataStream s(&_inputfile);
        s.setByteOrder( QDataStream::LittleEndian );
        s.readRawBytes(m_Header.type,2);
        if (!isBmp()) {
            _inputfile.close();
            return;
        }
        s >> m_Header.hSize;
        s >> m_Header.reserved1 >> m_Header.reserved2;
        s >> m_Header.Size;
        if ( m_Header.Size == BmpHeader::WIN || m_Header.Size == BmpHeader::OS2 ) {
            s >> m_Header.Width >> m_Header.Height >> m_Header.Planes >> m_Header.BitCount;
            s >> m_Header.Compression >> m_Header.SizeImage;
            s >> m_Header.XPerMeter >> m_Header.YPerMeter;
            s >> m_Header.ClrUsed >> m_Header.ClrImportant;
        } else {
            Q_INT16 w, h;
            s >> w >> h >> m_Header.Planes >> m_Header.BitCount;
            m_Header.Width  = w;
            m_Header.Height = h;
            m_Header.Compression = BmpHeader::RGB;
            m_Header.SizeImage = 0;
            m_Header.XPerMeter = m_Header.YPerMeter = 0;
            m_Header.ClrUsed = m_Header.ClrImportant = 0;
        }
        _inputfile.close();
    }

    BmpHeader::~BmpHeader() {
    }
}


BmpSlave::BmpSlave()
    : SlaveInterface(QStringList("bmp"))
{}

BmpSlave::~BmpSlave() {

}

QString BmpSlave::iconViewName(const QString& str) {
    QString st;
    BmpHeader bh(str);
    if (!bh.isBmp()) {
        st.append("No bmp file");
        return st;
    }
    QSize isize = bh.imageSize();
    st+=QObject::tr("Dimensions: %1 x %2\n").arg(isize.width()).arg(isize.height());
    st+=QObject::tr("Size: %1\n").arg(bh.Size());
    st+=QObject::tr("Depth: %1\n").arg(bh.bitsPixel());
    return st;
}

QString BmpSlave::fullImageInfo( const QString& str) {
    QString st = "<qt>";
    BmpHeader bh(str);
    if (!bh.isBmp()) {
        st.append("No bmp file");
        st.append(  "</qt>" );
        return st;
    }
    QSize isize = bh.imageSize();
    st+=QObject::tr("Dimensions: %1 x %2\n").arg(isize.width()).arg(isize.height());
    st+=QObject::tr("Size: %1\n").arg(bh.Size());
    st+=QObject::tr("Compression: %1\n").arg(bh.imageCompression());
    if (bh.isCompressed()) {
        st+=QObject::tr("Compressed size: %1").arg(bh.compressedSize());
    }
    st+=QObject::tr("Depth: %1\n").arg(bh.bitsPixel());
    st+=QObject::tr("used colors: %1\n").arg(bh.ColorsUsed());
    st+=QObject::tr("Resolution: %1 x %2\n").arg(bh.XPix()).arg(bh.YPix());
    st.append(  "</qt>" );
    return st;
}

QPixmap BmpSlave::pixmap(const QString& path, int width, int height ) {
    static QImage img;
    img.load( path );
    if ( img.isNull() ) {
        QPixmap pix;
        return pix;
    }

    return ThumbNailTool::scaleImage( img, width,height );
}
