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
#include "newpagedialog.h"

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

DrawPadCanvas::DrawPadCanvas(DrawPad* drawPad, QWidget* parent, const char* name)
    : QScrollView(parent, name)
{
    m_pDrawPad = drawPad;
    m_pages.setAutoDelete(true);
    m_pageBackups.setAutoDelete(true);
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

    m_pages = drawPadCanvasXmlHandler.pixmaps();

    if (m_pages.isEmpty()) {
        m_pages.append(new QPixmap(contentsRect().size()));
        m_pages.current()->fill(Qt::white);
    }

    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

void DrawPadCanvas::initialPage()
{
    m_pages.append(new QPixmap(234, 252));
    m_pages.current()->fill(Qt::white);

    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

void DrawPadCanvas::save(QIODevice* ioDevice)
{
    QTextStream textStream(ioDevice);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));

    textStream << "<drawpad>" << endl;
    textStream << "    <images>" << endl;

    QListIterator<QPixmap> bufferIterator(m_pages);

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
    return m_pages.current();
}

uint DrawPadCanvas::pagePosition()
{
    return (m_pages.at() + 1);
}

uint DrawPadCanvas::pageCount()
{
    return m_pages.count();
}

void DrawPadCanvas::clearAll()
{
    m_pages.clear();

    m_pages.append(new QPixmap(contentsRect().size()));
    m_pages.current()->fill(Qt::white);

    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

void DrawPadCanvas::newPage()
{
    QRect rect = contentsRect();

    NewPageDialog newPageDialog(this);
    newPageDialog.setWidth(rect.width());
    newPageDialog.setHeight(rect.height());

    if (newPageDialog.exec() == QDialog::Accepted) {
        m_pages.insert(m_pages.at() + 1, new QPixmap(newPageDialog.width(), newPageDialog.height()));
        m_pages.current()->fill(Qt::white);

        m_pageBackups.clear();
        m_pageBackups.append(new QPixmap(*(m_pages.current())));

        resizeContents(m_pages.current()->width(), m_pages.current()->height());
        viewport()->update();

        emit pagesChanged();
        emit pageBackupsChanged();
    }
}

void DrawPadCanvas::clearPage()
{
    m_pages.current()->fill(Qt::white);

    viewport()->update();
}

void DrawPadCanvas::deletePage()
{
    m_pages.remove(m_pages.current());

    if (m_pages.isEmpty()) {
        m_pages.append(new QPixmap(contentsRect().size()));
        m_pages.current()->fill(Qt::white);
    }

    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

bool DrawPadCanvas::undoEnabled()
{
    return (m_pageBackups.current() != m_pageBackups.getFirst());
}

bool DrawPadCanvas::redoEnabled()
{
    return (m_pageBackups.current() != m_pageBackups.getLast());
}

bool DrawPadCanvas::goPreviousPageEnabled()
{
    return (m_pages.current() != m_pages.getFirst());
}

bool DrawPadCanvas::goNextPageEnabled()
{
    return (m_pages.current() != m_pages.getLast());
}

void DrawPadCanvas::undo()
{
    *(m_pages.current()) = *(m_pageBackups.prev());

    viewport()->update();

    emit pageBackupsChanged();
}

void DrawPadCanvas::redo()
{
    *(m_pages.current()) = *(m_pageBackups.next());

    viewport()->update();

    emit pageBackupsChanged();
}

void DrawPadCanvas::goFirstPage()
{
    m_pages.first();
    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

void DrawPadCanvas::goPreviousPage()
{
    m_pages.prev();
    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

void DrawPadCanvas::goNextPage()
{
    m_pages.next();
    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

void DrawPadCanvas::goLastPage()
{
    m_pages.last();
    m_pageBackups.clear();
    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    resizeContents(m_pages.current()->width(), m_pages.current()->height());
    viewport()->update();

    emit pagesChanged();
    emit pageBackupsChanged();
}

void DrawPadCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    m_pDrawPad->drawMode()->mousePressEvent(e);
}

void DrawPadCanvas::contentsMouseReleaseEvent(QMouseEvent* e)
{
    m_pDrawPad->drawMode()->mouseReleaseEvent(e);

    QPixmap* currentBackup = m_pageBackups.current();
    while (m_pageBackups.last() != currentBackup) {
        m_pageBackups.removeLast();
    }

    while (m_pageBackups.count() >= (5 + 1)) {
        m_pageBackups.removeFirst();
    }

    m_pageBackups.append(new QPixmap(*(m_pages.current())));

    emit pageBackupsChanged();
}

void DrawPadCanvas::contentsMouseMoveEvent(QMouseEvent* e)
{
    m_pDrawPad->drawMode()->mouseMoveEvent(e);
}

void DrawPadCanvas::drawContents(QPainter* p, int cx, int cy, int cw, int ch)
{
    QRect clipRect(cx, cy, cw, ch);

    p->drawPixmap(clipRect.topLeft(), *(m_pages.current()), clipRect);
}
