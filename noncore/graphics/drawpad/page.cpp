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

Page::Page()
    : QPixmap()
{
    m_lastModified = QDateTime::currentDateTime();
}

Page::Page(int w, int h)
    : QPixmap(w, h)
{
    m_lastModified = QDateTime::currentDateTime();
}

Page::Page(const QSize& size)
    : QPixmap(size)
{
    m_lastModified = QDateTime::currentDateTime();
}

Page::~Page()
{
}

QString Page::title() const
{
    return m_title;
}

QDateTime Page::lastModified() const
{
    return m_lastModified;
}

void Page::setTitle(QString title)
{
    m_title = title;
}

void Page::setLastModified(QDateTime lastModified)
{
    m_lastModified = lastModified;
}

