/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "drawpadcanvas.h"

#include "drawmode.h"
#include "drawpad.h"

#include <qbuffer.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtextcodec.h>
#include <qtextstream.h>
#include <qxml.h>

#include <zlib.h>

class DrawPadCanvasXmlHandler: public QXmlDefaultHandler
{
public:
    DrawPadCanvasXmlHandler();
    ~DrawPadCanvasXmlHandler();

    QList<QPixmap> pixmaps();

    bool startElement(const QString& namespaceURI, const QString& localName,
                      const QString& qName, const QXmlAttributes& atts);
    bool endElement(const QString& namespaceURI, const QString& localName,
                    const QString& qName);
    bool characters(const QString& ch);

private:
    enum State {
        Unknown,
        InData
    };

    State m_state;
    ulong m_dataLenght;
    QList<QPixmap> m_pixmaps;
};

DrawPadCanvasXmlHandler::DrawPadCanvasXmlHandler()
{
    m_state = Unknown;
}

DrawPadCanvasXmlHandler::~DrawPadCanvasXmlHandler()
{
}

QList<QPixmap> DrawPadCanvasXmlHandler::pixmaps()
{
    return m_pixmaps;
}

bool DrawPadCanvasXmlHandler::startElement(const QString& namespaceURI, const QString& localName,
                                           const QString& qName, const QXmlAttributes& atts)
{
    Q_CONST_UNUSED(namespaceURI)
    Q_CONST_UNUSED(localName)

    if (qName.compare("data") == 0) {
        m_state = InData;
        m_dataLenght = atts.value("length").toULong();
    }

    return true;
}

bool DrawPadCanvasXmlHandler::endElement(const QString& namespaceURI, const QString& localName,
                                         const QString& qName)
{
    Q_CONST_UNUSED(namespaceURI)
    Q_CONST_UNUSED(localName)

    if (qName.compare("data") == 0) {
        m_state = Unknown;
    }

    return true;
}

bool DrawPadCanvasXmlHandler::characters(const QString& ch) 
{
    if (m_state == InData) {
        QByteArray byteArray(ch.length() / 2);

        for (int i = 0; i < (int)ch.length() / 2; i++) {
            char h = ch[2 * i].latin1();
            char l = ch[2 * i  + 1].latin1();
            uchar r = 0;

            if (h <= '9') {
                r += h - '0';
            } else {
                r += h - 'a' + 10;
            }

            r = r << 4;

            if (l <= '9') {
                r += l - '0';
            } else {
                r += l - 'a' + 10;
            }

            byteArray[i] = r;
        }

        if (m_dataLenght < ch.length() * 5) {
            m_dataLenght = ch.length() * 5;
        }

        QByteArray byteArrayUnzipped(m_dataLenght);
        ::uncompress((uchar*)byteArrayUnzipped.data(), &m_dataLenght, (uchar*)byteArray.data(), byteArray.size());

        QImage image;
        image.loadFromData((const uchar*)byteArrayUnzipped.data(), m_dataLenght, "XPM");

        QPixmap* pixmap = new QPixmap(image.width(), image.height());
        pixmap->convertFromImage(image);
        m_pixmaps.append(pixmap);
    }

    return true;
}

DrawPadCanvas::DrawPadCanvas(DrawPad* drawPad, QWidget* parent, const char* name, WFlags f)
    : QWidget(parent, name, f)
{
    setBackgroundMode(QWidget::PaletteBase);

    m_pDrawPad = drawPad;
    m_buffers.setAutoDelete(true);
    m_buffers.append(new QPixmap(width(), height()));
    m_buffers.current()->fill(Qt::white);
}

DrawPadCanvas::~DrawPadCanvas()
{
}

void DrawPadCanvas::load(QIODevice* ioDevice)
{
    QTextStream textStream(ioDevice);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));

    QXmlInputSource xmlInputSource(textStream);
    QXmlSimpleReader xmlSimpleReader;
    DrawPadCanvasXmlHandler drawPadCanvasXmlHandler;

    xmlSimpleReader.setContentHandler(&drawPadCanvasXmlHandler);
    xmlSimpleReader.parse(xmlInputSource);

    m_buffers = drawPadCanvasXmlHandler.pixmaps();

    if (m_buffers.isEmpty()) {
        m_buffers.append(new QPixmap(width(), height()));
        m_buffers.current()->fill(Qt::white);
    }

    repaint();
}

void DrawPadCanvas::save(QIODevice* ioDevice)
{
    QTextStream textStream(ioDevice);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));

    textStream << "<drawpad>" << endl;
    textStream << "    <images>" << endl;

    QListIterator<QPixmap> bufferIterator(m_buffers);

    for (bufferIterator.toFirst(); bufferIterator.current() != 0; ++bufferIterator) {
        textStream << "        <image>" << endl;

        QImage image = bufferIterator.current()->convertToImage();
        QByteArray byteArray;
        QBuffer buffer(byteArray);
        QImageIO imageIO(&buffer, "XPM");

        buffer.open(IO_WriteOnly);
        imageIO.setImage(image);
        imageIO.write();
        buffer.close();

        ulong size = byteArray.size() * 2;
        QByteArray byteArrayZipped(size);
        ::compress((uchar*)byteArrayZipped.data(), &size, (uchar*)byteArray.data(), byteArray.size());

        textStream << "            <data length=\"" << byteArray.size() << "\">";

        static const char hexchars[] = "0123456789abcdef";

        for (int i = 0; i < (int)size; i++ ) {
            uchar s = (uchar)byteArrayZipped[i];
            textStream << hexchars[s >> 4];
            textStream << hexchars[s & 0x0f];
        }

        textStream << "</data>" << endl;
        textStream << "        </image>" << endl;
    }

    textStream << "    </images>" << endl;
    textStream << "</drawpad>";
}

QPixmap* DrawPadCanvas::currentPage()
{
    return m_buffers.current();
}

void DrawPadCanvas::clearAll()
{
    m_buffers.clear();

    m_buffers.append(new QPixmap(width(), height()));
    m_buffers.current()->fill(Qt::white);

    repaint();
}

void DrawPadCanvas::newPage()
{
    m_buffers.insert(m_buffers.at() + 1, new QPixmap(width(), height()));
    m_buffers.current()->fill(Qt::white);
    repaint();
}

void DrawPadCanvas::clearPage()
{
    m_buffers.current()->fill(Qt::white);
    repaint();
}

void DrawPadCanvas::deletePage()
{
    m_buffers.remove(m_buffers.current());

    if (m_buffers.isEmpty()) {
        m_buffers.append(new QPixmap(width(), height()));
        m_buffers.current()->fill(Qt::white);
    }

    repaint();
}

bool DrawPadCanvas::goPreviousPageEnabled()
{
    return (m_buffers.current() != m_buffers.getFirst());
}

bool DrawPadCanvas::goNextPageEnabled()
{
    return (m_buffers.current() != m_buffers.getLast());
}

void DrawPadCanvas::goFirstPage()
{
    m_buffers.first();
    repaint();
}

void DrawPadCanvas::goPreviousPage()
{
    m_buffers.prev();
    repaint();
}

void DrawPadCanvas::goNextPage()
{
    m_buffers.next();
    repaint();
}

void DrawPadCanvas::goLastPage()
{
    m_buffers.last();
    repaint();
}

void DrawPadCanvas::mousePressEvent(QMouseEvent* e)
{
    m_pDrawPad->drawMode()->mousePressEvent(e);
}

void DrawPadCanvas::mouseReleaseEvent(QMouseEvent* e)
{
    m_pDrawPad->drawMode()->mouseReleaseEvent(e);
}

void DrawPadCanvas::mouseMoveEvent(QMouseEvent* e)
{
    m_pDrawPad->drawMode()->mouseMoveEvent(e);
}

void DrawPadCanvas::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    QListIterator<QPixmap> bufferIterator(m_buffers);

    for (bufferIterator.toFirst(); bufferIterator.current() != 0; ++bufferIterator) {
        int w = width() > bufferIterator.current()->width() ? width() : bufferIterator.current()->width();
        int h = height() > bufferIterator.current()->height() ? height() : bufferIterator.current()->height();

        QPixmap tmpPixmap(*(bufferIterator.current()));
        bufferIterator.current()->resize(w, h);
        bufferIterator.current()->fill(Qt::white);

        bitBlt(bufferIterator.current(), 0, 0, &tmpPixmap, 0, 0, tmpPixmap.width(), tmpPixmap.height());
    }
}

void DrawPadCanvas::paintEvent(QPaintEvent* e)
{
    QWidget::paintEvent(e);

    QArray<QRect> rects = e->region().rects();

    for (uint i = 0; i < rects.count(); i++) {
        QRect r = rects[i];
        bitBlt(this, r.x(), r.y(), m_buffers.current(), r.x(), r.y(), r.width(), r.height());
    }
}
