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

#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <qdialog.h>

#include <qlist.h>

class QListView;

class Page;

class ThumbnailView : public QDialog
{ 
    Q_OBJECT

public:
    ThumbnailView(QList<Page> pages, QWidget* parent = 0, const char* name = 0);
    ~ThumbnailView();

protected:
    void resizeEvent(QResizeEvent* e);

private:
    QListView* m_pListView;
};

#endif // THUMBNAILVIEW_H
