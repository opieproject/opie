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

#include "page.h"

const int PAGE_BACKUPS = 99;

Page::Page()
{
    m_title = "";
    m_lastModified = QDateTime::currentDateTime();
    m_pPixmap = new QPixmap();

    m_backHistory.setAutoDelete(true);
    m_forwardHistory.setAutoDelete(true);
}

Page::Page(QString title, int w, int h)
{
    m_title = title;
    m_lastModified = QDateTime::currentDateTime();
    m_pPixmap = new QPixmap(w, h);

    m_backHistory.setAutoDelete(true);
    m_forwardHistory.setAutoDelete(true);
}

Page::Page(QString title, const QSize& size)
{
    m_title = title;
    m_lastModified = QDateTime::currentDateTime();
    m_pPixmap = new QPixmap(size);

    m_backHistory.setAutoDelete(true);
    m_forwardHistory.setAutoDelete(true);
}

Page::~Page()
{
    delete m_pPixmap;
}

QString Page::title() const
{
    return m_title;
}

QDateTime Page::lastModified() const
{
    return m_lastModified;
}

QPixmap* Page::pixmap() const

{
    return m_pPixmap;
}

void Page::setTitle(QString title)
{
    m_title = title;
}

void Page::setLastModified(QDateTime lastModified)
{
    m_lastModified = lastModified;
}

bool Page::undoEnabled()
{
    return (!m_backHistory.isEmpty());
}

bool Page::redoEnabled()
{
    return (!m_forwardHistory.isEmpty());
}

void Page::backup()
{
    setLastModified(QDateTime::currentDateTime());

    while (m_backHistory.count() >= (PAGE_BACKUPS + 1)) {
        m_backHistory.removeFirst();
    }

    m_backHistory.append(new QPixmap(*m_pPixmap));
    m_forwardHistory.clear();
}

void Page::undo()
{
    m_forwardHistory.append(new QPixmap(*m_pPixmap));
    m_pPixmap = new QPixmap(*(m_backHistory.last()));
    m_backHistory.removeLast();
}

void Page::redo()
{
    m_backHistory.append(new QPixmap(*m_pPixmap));
    m_pPixmap = new QPixmap(*(m_forwardHistory.last()));
    m_forwardHistory.removeLast();
}
