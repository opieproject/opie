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

#ifndef PAGEINFORMATIONDIALOG_H
#define PAGEINFORMATIONDIALOG_H

#include <qdialog.h>

class Page;

class QLineEdit;

class PageInformationDialog : public QDialog
{
    Q_OBJECT

public:
    PageInformationDialog(Page* page, QWidget* parent = 0, const char* name = 0);
    ~PageInformationDialog();

    QString selectedTitle();

private:
    Page* m_pPage;

    QLineEdit* m_pTitleLineEdit;
};

#endif // PAGEINFORMATIONDIALOG_H
