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

#ifndef PAGE_H
#define PAGE_H

#include <qpixmap.h>

#include <qdatetime.h>

class Page : public QPixmap
{
public:
    Page();
    Page(int w, int h);
    Page(const QSize& size);

    ~Page();
    
    QString title() const;
    QDateTime lastModified() const;

    void setTitle(QString title);
    void setLastModified(QDateTime lastModified);

private:
    QString m_title;
    QDateTime m_lastModified;
};

#endif // PAGE_H
