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

#include <qobject.h>

#include <qdatetime.h>
#include <qlist.h>
#include <qpixmap.h>

class Page : public QObject
{
public:
    Page();
    Page(QString title, int w, int h);
    Page(QString title, const QSize& size);

    ~Page();

    QString title() const;
    QDateTime lastModified() const;
    QPixmap* pixmap() const;

    void setTitle(QString title);
    void setLastModified(QDateTime lastModified);

    bool undoEnabled();
    bool redoEnabled();

    void backup();
    void undo();
    void redo();

private:
    QString m_title;
    QDateTime m_lastModified;
    QPixmap* m_pPixmap;

    QList<QPixmap> m_backHistory;
    QList<QPixmap> m_forwardHistory;
};

#endif // PAGE_H
